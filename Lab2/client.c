#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
int64_t now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL); // 获取当前精确时间
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
int main(int argc, char *argv[])
{
	
	unsigned short port = 9999;             // 服务器的端口号
	char *server_ip = "127.0.0.1";    	// 服务器ip地址
	
	if( argc > 1 )	        //函数传参，可以更改服务器地址				
	{		
		server_ip = argv[1];
	}	
	if( argc > 2 )	       //函数传参，可以更改服务器端口号		
	{
		port = atoi(argv[2]);
	}
 	struct timeval tv;
	
	int sockfd;             //定义套接字连接
	int64_t start = now();//开始计时

	sockfd = socket(AF_INET, SOCK_STREAM, 0);// 创建通信端点：套接字
	if(sockfd < 0)         //套接字连接失败
	{
		perror("socket");
		exit(-1);
	}
	
	// 设置服务器地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;	// IPv4
	server_addr.sin_port = htons(port);	// 端口
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);	// ip
	
	 // 主动连接服务器
	int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));     
	if(err_log != 0)                       //服务器连接失败
	{
		perror("connect");
		close(sockfd);
		exit(-1);
	}
	
	int recv_len = 0;                      //缓冲区长度
	//char send_buf[512] = {0};              //定义要发送数据
        char recv_buf[512] = {0};              //定义要接收数据
	//int connfd = *(int *)arg; // 传过来的已连接套接字
	printf("send data to %s:%d\n",server_ip,port);  //打印数据发送服务器
        int n=0;
	//char i[512]={0};
	printf("**************************************************\n");
	while(1)
	{
		
		printf("--------------------------%d\n",n);
		
		if(n%2==0)
		{
			
			char send_buf[512]= "GET /index.html HTTP/1.1\r\nUser-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3\r\nHost: 127.0.0.1:9999\r\nAccept: */*\r\n\r\n";
			printf("send_buf: %s\n", send_buf);
			send(sockfd, send_buf, strlen(send_buf), 0);   // 向服务器发送信息
		
              		//recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
                	if(recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0)>0)
			{
				printf("***recv_buf: %s\n", recv_buf); // 打印接收的数据
			}       
			n++;
		}
		else
		{
			char send_buf[512]= "POST /Post_show HTTP/1.1\r\nUser-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3\r\nHost: 127.0.0.1:9999\r\nAccept: */*\r\nContent-Length: 19\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
			printf("send_buf: %s\n", send_buf);
			send(sockfd, send_buf, strlen(send_buf), 0);   // 向服务器发送信息
		
                	//recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
                	if(recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0)>0)
			{
				printf("***recv_buf: %s\n", recv_buf); // 打印接收的数据
			}       
			n++;		
		}
                
		int64_t end = now();//停止计时
    		double sec = (end - start) / 1000000.0;
		if(sec>40)
		{
			break;
		}
	}
 
	close(sockfd);
	
	return 0;
}

