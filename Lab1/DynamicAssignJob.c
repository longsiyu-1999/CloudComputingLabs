#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
DynamicAssignJob.c
#define INPUT_JOB_NUM 10 //任务总数
#define INPUT_FOLDER_NAME "testInput"

#define THE_HEAVY_JOB_ID 1 //耗时较长的任务ID
int THE_HEAVY_WEIGHT = 1; //耗时较长的任务权重

#define CHUNK_SIZE 4096 //块大小
#define TOTAL_CHUNK_NUM 10240  //总的块数量

typedef unsigned char BYTE;

char inJob[INPUT_JOB_NUM][256];
int nextJobToBeDone = 0;
pthread_mutex_t jobQueueMutex = PTHREAD_MUTEX_INITIALIZER;//任务队列初始化

double time_diff(struct timeval x, struct timeval y)
{
    double x_ms, y_ms, diff;
    x_ms = (double)x.tv_sec * 1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec * 1000000 + (double)y.tv_usec;
    diff = (double)y_ms - (double)x_ms;
    if (diff < 0)
    {
        fprintf(stderr, "ERROR! time_diff<0\n");
        exit(1);
    }
    return diff;
}

long int generateJobs()//生成任务
{
    char command[1000];
    sprintf(command, "rm -rf %s", INPUT_FOLDER_NAME);
    int status = system(command);
    mkdir(INPUT_FOLDER_NAME, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //创建目录
    BYTE writeBuf[CHUNK_SIZE]; //写缓存
    int writeSize = 0;
    for (int i = 0; i < CHUNK_SIZE; i++)
        writeBuf[i]= 1; //All byte set to be 1
    long int totalBytes = 0;
    for (int i = 0; i < INPUT_JOB_NUM; i++)
    {
        sprintf(inJob[i], "%s/testInput%04d.txt", INPUT_FOLDER_NAME, i); //将格式化的字符写入txt文件中
        FILE *fp;
        if ((fp = fopen(inJob[i], "w")) == NULL) //如果打开文件错误
        {
            perror("fopen ERROR!");//报错并退出
            exit(1);
        }
        int heavyScale = 1;
        if (i == THE_HEAVY_JOB_ID) //如果当前任务是耗时较长的任务
            heavyScale = THE_HEAVY_WEIGHT;//将权重规模改成之前设置的重的权重
        for (int j = 0; j < TOTAL_CHUNK_NUM * heavyScale; j++)//将数据写入文件

            while (1) //Write until this CHUNK_SIZE has all been written
            { //fwrite是文件处理函数，向指定的文件中写入若干数据块
                writeSize = writeSize + fwrite(writeBuf, 1, CHUNK_SIZE - writeSize, fp);
                totalBytes = totalBytes + writeSize;
                if (writeSize < 0)
                {
                    perror("write ERROR!");
                    exit(1);
                }
                else if (writeSize == CHUNK_SIZE)
                {
                    //This CHUNK_SIZE done, go to the next CHUNK_SIZE
                    writeSize = 0;
                    break;
                }
            }
        }
        fclose(fp);
    }
    return totalBytes;
}

typedef struct
{
    long int result;
} ThreadParas;

int recvAJob()//拿之前要加锁
{
    int currentJobID = 0; //当前待拿走的任务
    pthread_mutex_lock(&jobQueueMutex);//加锁
    if (nextJobToBeDone >= INPUT_JOB_NUM)//看全部任务是否做完
    {
        pthread_mutex_unlock(&jobQueueMutex);//解锁
        return -1;
    }
    currentJobID = nextJobToBeDone;//当前的任务已被拿走
    nextJobToBeDone++;//后面的若要拿就得拿下一个
    pthread_mutex_unlock(&jobQueueMutex);//解锁
    return currentJobID;//返回当前任务序号
}

void processAJob(int jobID, long int *sum)
{
    BYTE readBuf[CHUNK_SIZE] = {0};
    int readSize = 0;
    FILE *fp;
    if ((fp = fopen(inJob[jobID], "r")) == NULL)//如果文件为空
    {
        perror("fopen ERROR!");//报错
        exit(1);
    }
    while (1) //Read until EOF//读文件数据
    {
        readSize = fread(readBuf, 1, CHUNK_SIZE, fp);
        if (readSize < 0)//如果大小小于0
        {
            perror("read ERROR!");//报错
            exit(1);
        }
        else if (readSize == 0)//如果大小等于0
        { //EOF
            break;//跳出循环
        }
        for (int j = 0; j < readSize; j++)//累加求和
            *sum = *sum + readBuf[j];
        memset(readBuf, 0, sizeof(BYTE) * readSize);
    }
    fclose(fp);
}

void *calcSum(void *args)
{
    ThreadParas *para = (ThreadParas *)args;
    long int sum = 0;//总和初始化
    int currentJobID = 0;
    int *whichJobIHaveDone = malloc(INPUT_JOB_NUM * sizeof(int)); //Remember which job I have done
    long int numOfJobsIHaveDone = 0;                              //Remember how many jobs I have done
    while (1)
    {
        currentJobID = recvAJob();//拿走后把ID返回给线程调用函数
        if (currentJobID == -1) //All job done!
            break;

        whichJobIHaveDone[numOfJobsIHaveDone] = currentJobID;
        numOfJobsIHaveDone++;

        processAJob(currentJobID, &sum);//拿了之后就开始做
    }
    // pthread_t tid = pthread_self();
    // printf("[%ld] thread (sum of inJobs[", pthread_self());
    // for (int i = 0; i < numOfJobsIHaveDone; i++)
    // {
    //     if (i > 0)
    //         printf(",");
    //     printf("%d", whichJobIHaveDone[i]);
    // }
    // printf("]): \t %ld\n", sum);
    para->result = sum;
}

int main(int argc, char *argv[])
{
    int numOfWorkerThread = 1;
    if (argc >= 2)//参数2是线程数
        numOfWorkerThread = atoi(argv[1]);
    if (numOfWorkerThread > INPUT_JOB_NUM)
        numOfWorkerThread = INPUT_JOB_NUM;
    if (argc >= 3)//参数3是权重大小
        THE_HEAVY_WEIGHT = atoi(argv[2]);

    // struct timeval tvGenStart, tvEnd;
    // struct timeval tvMainStartCacl, tvMainEndCacl;
    // struct timeval tvWorkerStartCacl, tvWorkerEndCacl;

    // printf("Generating input jobs ...\n");
    // gettimeofday(&tvGenStart, NULL);
    long int totalBytes = generateJobs();
    // gettimeofday(&tvEnd, NULL);
    // printf("Generating input jobs done. Spend %.5lf s to finish. Total test input data size is %lf MBs\n", time_diff(tvGenStart, tvEnd) / 1E6, (double)totalBytes / 1E6);

    // printf("Main thread start doing jobs ...\n");
    // gettimeofday(&tvMainStartCacl, NULL);
    ThreadParas thParaMain;
    calcSum(&thParaMain);
    // gettimeofday(&tvMainEndCacl, NULL);
    // printf("Main thread finish jobs. Spend %.5lf s to finish.\n", time_diff(tvMainStartCacl, tvMainEndCacl) / 1E6);

    nextJobToBeDone = 0;
    // printf("Worker threads start doing jobs ...\n");
    // gettimeofday(&tvWorkerStartCacl, NULL);
    pthread_t th[numOfWorkerThread];
    ThreadParas thPara[numOfWorkerThread];
    for (int i = 0; i < numOfWorkerThread; i++)
    {
        if (pthread_create(&th[i], NULL, calcSum, &thPara[i]) != 0)//创建线程
        {
            perror("pthread_create failed");
            exit(1);
        }
    }
    for (int i = 0; i < numOfWorkerThread; i++)//循环
        pthread_join(th[i], NULL);//等待线程结束 同步操作
    // gettimeofday(&tvWorkerEndCacl, NULL);
    // printf("Worker threads finish jobs. Spend %.5lf s to finish.\n", time_diff(tvWorkerStartCacl, tvWorkerEndCacl) / 1E6);

    long int workerSum = 0;
    for (int i = 0; i < numOfWorkerThread; i++)
        workerSum = workerSum + thPara[i].result;//累加
    // printf("Sum of all %d threads: \t\t %ld\n", numOfWorkerThread, workerSum);

    //In real project, do free the memory and destroy mutexes and semaphores
    exit(0);
}