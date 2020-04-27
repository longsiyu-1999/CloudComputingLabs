#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>					
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>					
#include <pthread.h>
#include <iostream>
#include <string>
#include <string.h>
#include<fcntl.h>
using namespace std;
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
	int connfd = *(int *)arg; // 传过来的已连接套接字
	
	// 解锁，pthread_mutex_lock()唤醒，不阻塞
	pthread_mutex_unlock(&mutex); 
	
	// 接收数据
	if((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0)
	{
		printf("recv_buf: %s\n", recv_buf); // 打印数据
		send(connfd, recv_buf, recv_len, 0); // 给客户端回数据
	}
	
	printf("client closed!\n");
	close(connfd);	//关闭已连接套接字
	
	return 	NULL;
}

void Error_dealer(string method,string url)
{
	string entity;
	if(method!="GET"&&method!="POST"){//情况1：除GET和POST之外的请求 返回501
		entity="<htmL><title>501 Not Implemented</title><body bgcolor=ffffff>\n Not Implemented\n<p>Does not implement this method: "+method+"\n,<hr><em>HTTP Web Server </em>\n</body></html>\n";
		string tmp="Http/1.1 501 Not Implemented\r\nContent-type: text/html\r\nContent-length:"+to_string(entity.length())+"\r\n\r\n";
		string message=tmp+entity;
		printf("%s",message.c_str());
	}
	else{
		if(method=="GET"){//情况2：GET请求但不满足要求 返回404
			entity="<html><title>404 Not Found</title><body bgcolor=ffffff>\n Not Found\n<p>Couldn't find this file: "+url+"\n<hr><em>HTTP Web Sever</em>\n</body></html>\n";
		}
		else if(method=="POST"){//情况3：POST请求但不满足要求 返回404
			entity="<html><title>404 Not Found</title><body bgcolor=ffffff>\n Not Found\n<hr><em>HTTP Web Sever</em>\n</body></html>\n";
		}
		string tmp="Http/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-length:"+to_string(entity.length())+"\r\n\r\n";
		string message=tmp+entity;
		printf("%s",message.c_str());
	}
		
}

void Get_dealer(string method,string url){
	int len=url.length();
	string tmp="./src";
	if(url.find(".")==string::npos){//如果url中不存在‘.’
		if(url[len-1]=='/'||url.length()==0){
			tmp+=url+"index.html";
		}
		else tmp+=url+"./index.html";
	}
	else tmp+=url;
	//cout<<tmp<<endl;
	int fd=open(tmp.c_str(),O_RDONLY);//若所有欲核查的权限都通过了检查则返回0值，表示成功，只要有一个权限被禁止则返回-1。
	if(fd>=0){
		printf("Http/1.1 200 OK\r\nContent-length:%d\r\nContent-type: text/html\r\n\r\n",tmp.length());
	}
	else{
		Error_dealer(method,url);
	}
}

void Post_dealer(string name,string ID){
	string entity="<html><title>POST Method</title><body bgcolor=ffffff>\nYour Name: "+name+"\nID: "+ID+"\n<hr><em>Http Web server</em>\n</body></html>\n";
	int ilen=entity.length();
	string slen;
	sprintf(slen,"%d",ilen);
	string tmp="HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: "+slen+"\r\n\r\n";
	string message=tmp+entity;
	printf("%s",message.c_str());
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
	unsigned short port = 8080; // 监听端口

	//测试行
	//string a="hello, nice to meet you.\n You are my sunshine\n";
	//printf("%s",a.c_str());

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
		//const char *inet_ntop(int af, const void *src, char *dst, socklen_t cat);
		inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);//将数值格式转化为点分十进制的ip地址格式
		printf("inet_ntop:%s\n",cli_ip);
        //返回值：若成功则为指向结构的指针，若出错则为NULL
		printf("----------------------------------------------\n");
		printf("client ip=%s,port=%d\n", cli_ip,ntohs(client_addr.sin_port));
		
		if(connfd > 0)
		{
			//给回调函数传的参数，&connfd，地址传递
			pthread_create(&thread_id, NULL, client_process, &connfd);  //创建线程
			pthread_detach(thread_id); // 线程分离，结束时自动回收资源
		}
	}
	
	close(sockfd);
	
	return 0;
}

