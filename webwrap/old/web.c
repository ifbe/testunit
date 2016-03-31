#include<sys/socket.h>  
#include<errno.h>  
#include<netinet/in.h>  
#include<string.h>  
#include<stdio.h>  
#include<unistd.h>
#include<fcntl.h>
  
static char buf[2048];  
#define SERVER_PORT 8081
  
//定义好的html页面，实际情况下web server基本是从本地文件系统读取html文件   
const static char http_response[] = 
"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n"; 
const static char http_head[] = 
"<html><head><title>codetree</title></head><body>"
"<form method=\"get\" action=\"http://127.0.0.1:8081\" style=\"text-align:center;\">"
"<input type=\"text\" name=\"i\" style=\"width:40%;height:32px\"></input>"
"<input type=\"submit\" style=\"width:10%;height:32px\"></input>"
"</form><hr><pre>";
const static char http_context[] =
"hahahaha"; 
const static char http_tail[] =
"</pre></body></html>"; 

//HTTP请求解析   
void serve(int sockfd)
{ 
	char* filename;
	char* ret;

	read(sockfd, buf, 1024);  
	//其他HTTP请求处理，如POST，HEAD等 。这里我们只处理GET   
	if(strncmp(buf, "GET", 3) != 0)
	{  
		printf("unsupported request!\n");  
		return;  
	}

	//是GET请求
	filename = buf + 4;  
	ret = strchr(filename, ' ');  
	*ret = '\0';  

	//头
	write(sockfd, http_response, strlen(http_response)); 
	write(sockfd, http_head, strlen(http_head)); 
	//write(sockfd, http_context, strlen(http_context));

	//发给exe，把拿到的回复扔回去
	if(strncmp(filename,"/?i=",4)==0)
	{
		filename+=4;
		write(sockfd, filename, strlen(filename));
	}

	//尾巴
	write(sockfd, http_tail, strlen(http_tail)); 
}  
  
void main(){  
    int sockfd,err,newfd;  
    struct sockaddr_in addr;  
    //建立TCP套接字   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if(sockfd < 0){  
        perror("socket creation failed!\n");  
        return;  
    }  
    memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    //这里要注意，端口号一定要使用htons先转化为网络字节序，否则绑定的实际端口  
    //可能和你需要的不同   
    addr.sin_port = htons(SERVER_PORT);  
    addr.sin_addr.s_addr = INADDR_ANY;  
    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))){  
        perror("socket binding failed!\n");  
        return;  
    }  
    listen(sockfd, 128);  
    for(;;){  
        //不间断接收HTTP请求并处理，这里使用单线程，在实际情况下考虑到效率一般多线程   
        newfd = accept(sockfd, NULL, NULL);  
        serve(newfd);  
        close(newfd);  
    }  
}  
