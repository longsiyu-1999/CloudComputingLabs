#include <stdio.h>
#include <stdlib.h>
#include <string.h>						
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>					
#include <pthread.h>
 
pthread_mutex_t mutex;	// 定义互斥锁，全局变量
 
/************************************************************************
函数名称：	void *client_process(void *arg)
函数功能：	线程函数,处理客户信息
函数参数：	已连接套接字
函数返回：	无
************************************************************************/
void *client_process(void *arg)
{
	int recv_len = 0;
	char recv_buf[1024] = "";	// 接收缓冲区
        //string send_buf[1024] = "";
	int connfd = *(int *)arg; // 传过来的已连接套接字
	
	// 解锁，pthread_mutex_lock()唤醒，不阻塞
	pthread_mutex_unlock(&mutex); 
	
	// 接收数据
	while((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0)
	{
		printf("recv_buf: %s\n", recv_buf); // 打印数据
                char send_buf[1024]="HTTP/1.1 200 OK";
                printf("send_buf:%s\n",send_buf);
		send(connfd, recv_buf,recv_len, 0); // 给客户端回数据
	}
	
	printf("client closed!\n");
	close(connfd);	//关闭已连接套接字
	
	return 	NULL;
}
 
//===============================================================
// 语法格式：	void main(void)
// 实现功能：	主函数，建立一个TCP并发服务器
// 入口参数：	无
// 出口参数：	无
//===============================================================
int main(int argc, char *argv[])
{
	int sockfd = 0;				// 套接字
	int connfd = 0;
	int err_log = 0;
	struct sockaddr_in my_addr;	// 服务器地址结构体
	unsigned short port = 8888; // 监听端口
	pthread_t thread_id;
	
	pthread_mutex_init(&mutex, NULL); // 初始化互斥锁，互斥锁默认是打开的
	
	printf("TCP Server Started at port %d!\n", port);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);   // 创建TCP套接字
	if(sockfd < 0)
	{
		perror("socket error");
		exit(-1);
	}
	
	bzero(&my_addr, sizeof(my_addr));	   // 初始化服务器地址
	my_addr.sin_family = AF_INET;
	my_addr.sin_port   = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Binding server to port %d\n", port);
	
	// 绑定
	err_log = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if(err_log != 0)
	{
		perror("bind");
		close(sockfd);		
		exit(-1);
	}
	
	// 监听，套接字变被动
	err_log = listen(sockfd, 10);
	if( err_log != 0)
	{
		perror("listen");
		close(sockfd);		
		exit(-1);
	}
	
	printf("Waiting client...\n");
	
	while(1)
	{
		char cli_ip[INET_ADDRSTRLEN] = "";	   // 用于保存客户端IP地址
		struct sockaddr_in client_addr;		   // 用于保存客户端地址
		socklen_t cliaddr_len = sizeof(client_addr);   // 必须初始化!!!
		
		// 上锁，在没有解锁之前，pthread_mutex_lock()会阻塞
		pthread_mutex_lock(&mutex);	
		
		//获得一个已经建立的连接	
		connfd = accept(sockfd, (struct sockaddr*)&client_addr, &cliaddr_len);   							
		if(connfd < 0)
		{
			perror("accept this time");
			continue;
		}
		
		// 打印客户端的 ip 和端口
		inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
		printf("----------------------------------------------\n");
		printf("client ip=%s,port=%d\n", cli_ip,ntohs(client_addr.sin_port));
		
		if(connfd > 0)
		{
			//给回调函数传的参数，&connfd，地址传递
			pthread_create(&thread_id, NULL, (void *)client_process, (void *)&connfd);  //创建线程
			pthread_detach(thread_id); // 线程分离，结束时自动回收资源
		}
	}
	
	close(sockfd);
	
	return 0;
}
