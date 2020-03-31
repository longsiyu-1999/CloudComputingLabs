#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "sudoku.h"

#define TNUM 5//线程数

char Answer[Task_num][128];//存储答案
char puzzle[Task_num][128];//存储数独

int total=0;//数独总数
int Sid=0;//当前的数独序号
bool shutdown=false;//

int Tdata[Task_num];//线程运行函数的参数
pthread_t th[Task_num];//线程号
pthread_mutex_t mutex;//定义一个锁

int recvAJob()
{
    int currentJobID = 0;
    pthread_mutex_lock(&mutex);
    if (Sid>=total)
    {
        pthread_mutex_unlock(&mutex);
        return -1;//所有任务已解决
    }
    currentJobID=Sid++;
    if (currentJobID==total-1)
    {
        shutdown=true;
    }
    pthread_mutex_unlock(&mutex);
    return currentJobID;
}

void *Run(void *args)
{
    bool (*solve)(int,int[],int,int[])=solve_sudoku_basic;//使用basic算法
    int board[N];
    int spaces[N];
    while (!shutdown)
    {
        int id=recvAJob();
        if (id==-1)
            break;
        int t=input(puzzle[id],board,spaces);
        if (solve(0,board,t,spaces))
        {
            if (!solved(board))
                assert(0);
            for (int i=0;i<N;i++)//保存结果
                Answer[id][i] = char('0' + board[i]);
        }
        else
        {
            printf("No: %d,无解\n", id);
        }
    }
}

void Create()
{
    for (int i = 0; i < TNUM; i++)
    {
        if (pthread_create(&th[i], NULL, Run,(void*)&Tdata[i])!= 0)
        {
            perror("pthread_create failed");
            exit(1);
        }
    }
}

void End()
{
    for (int i = 0; i < TNUM; i++)
        pthread_join(th[i], NULL);
}

void Print()
{
    for(int i=0;i<total; i++)
        printf("%s\n",Answer[i]);
}

int64_t now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL); // 获取当前精确时间
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char *argv[])
{
    init_neighbors();
    int64_t start = now();//开始计时
    //FILE* fp = fopen(argv[1],"r");
    char file_path[32];//直接读取输入文件大小为32位字符
    char *_= fgets(file_path, sizeof file_path, stdin);
    char iin[strlen(file_path)];
    for(int o=0;o<strlen(file_path)-1;o++)
    {
        iin[o]=file_path[o];
    }
    iin[strlen(file_path) - 1] = '\0';   //表示字符串结束
    FILE *fp = fopen(iin, "r");// 打开文件
    
    Create();//创建线程
    while (fgets(puzzle[total], sizeof puzzle, fp) != NULL)
    {
        if (strlen(puzzle[total]) >= N)
        {
            pthread_mutex_lock(&mutex);
            total++;
            pthread_mutex_unlock(&mutex);
        }
    }
    End();//判断结束线程
    Print();//输出答案
    int64_t end = now();//停止计时
    double sec = (end - start) / 1000000.0;
    // printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total); // 输出运行时间

    return 0;
}
