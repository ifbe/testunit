#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
//
static int childear[2];
static int childmouse[2];




void initmywebserver(int* sockfd,char* url)
{
	//
	static int serverport=8080;
	struct sockaddr_in addr;

	//
	for(ret=strlen(url)-1;ret>=0;ret--)
	{
		if(url[ret]!=':')continue;

		serverport=atoi(url+ret+1);
		break;
	}
	if( (serverport<0) | (serverport>65535) )
	{
		printf("wrong port:%d\n",serverport);
		exit(-1);
	}

	//建立TCP套接字   
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(*sockfd < 0)
	{
		perror("socket creation failed!\n");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	//这里要注意，端口号一定要使用htons先转化为网络字节序，否则绑定的实际端口  
	//可能和你需要的不同   
	addr.sin_port = htons(serverport);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(*sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
	{
		perror("socket binding failed!\n");
		exit(-1);
	}
	listen(*sockfd, 128);
}
void thisisfather(char* url)
{
	//temp
	int ret;
	int failcount;
	//socket related
	int sockfd;
	int thisfd;
	static unsigned char httpbuf[0x1000];
	//response text
	static unsigned char http_head[0x1000] = {0};
	static unsigned char http_tail[] = "</pre></body></html>";
	//from exe
	static int position=0;
	static unsigned char haha[0x100000*16]={0};




	//1.检查传进来的参数
	if(url==0)
	{
		snprintf(url,256,"http://127.0.0.1:8080");
	}
	snprintf(
		http_head,
		0x1000,

		"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
		"<html><head><title>codetree</title></head><body>"
		"<form method=\"get\" action=\"%s\" style=\"text-align:center;\">"
		"<input type=\"text\" name=\"i\" style=\"width:40%;height:32px\"></input>"
		"<input type=\"submit\" style=\"width:10%;height:32px\"></input>"
		"</form><hr><pre>",

		url
	);
	initmywebserver(&sockfd,url);




	//2.设置不阻塞，分别关掉两个单向管道的某一边
	//non block
	ret=fcntl(childmouse[0], F_GETFL);
	fcntl(childmouse[0],F_SETFL,ret|O_NONBLOCK);

	//parent only write to child's ear
	close(childear[0]);

	//parent only read from child's mouse
	close(childmouse[1]);




	//3.开始!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while(1)
	{
		thisfd = accept(sockfd, NULL, NULL);
		read(thisfd, httpbuf, 1024);

		//其他HTTP请求处理，如POST，HEAD等 。这里我们只处理GET   
		if(strncmp(httpbuf, "GET", 3) != 0)
		{
		        printf("unsupported request!\n");
		        goto nextone;
		}

		//发给exe，把拿到的回复扔回去
		if(strncmp(httpbuf+4,"/?i=",4)!=0)
		{
			position=0;
			goto sendresponse;
		}

		//string end = 0
		for(ret=8;ret<1000;ret++)
		{
			if(httpbuf[ret]<=0x20)
			{
				httpbuf[ret]=0;
				break;
			}
		}

		//发给exe
		ret=snprintf(haha,256,"%s\n",httpbuf+8);
		ret=write(childear[1],haha,ret);
		if(ret<=0)
		{
			printf("pipe closed\n");
			exit(-1);
		}

		//read from that program,write to stdout
		ret=0;
		failcount=0;
		position=0;
		while(1)
		{
			ret=read(childmouse[0],haha+position,0x1000);
			if(ret<0)
			{
				//printf("ret=%d,errno=%d\n",ret,errno);
				usleep(1000);
				fsync(childmouse[0]);

				failcount++;
				if(failcount<100)continue;
				else break;
			}
			else if(ret==0)
			{
				printf("pipe closed\n");
				exit(-1);
			}

			position+=ret;
			if(position>1000000)break;
		}
		haha[position]=0;
		//printf("%s",haha,position);

sendresponse:
		//头
		//write(thisfd, http_response, strlen(http_response));
		write(thisfd, http_head, strlen(http_head));

		//身体
		if(position!=0)
		{
			write(thisfd, haha, position);
		}

		//尾巴
		write(thisfd, http_tail, strlen(http_tail));

nextone:
		close(thisfd);
	}
}








void thisischild(int argc,char** argv)
{
	int ret;

	//子进程只输入，不输出
	dup2(childear[0],0);
	close(childear[1]);

	//子进程只输出，不输入
	dup2(childmouse[1],1);
	close(childmouse[0]);

	//bye bye
	ret=execvp(argv[0],argv);
	if(ret==-1)
	{
		//printf("execv fail:%d\n",errno);
		close(childear[0]);
		close(childmouse[1]);
		exit(-1);
	}
}








void main(int argc, char *argv[])
{
	int ii;
	int jj;
	char* p;
	char* url;
	char* finalargv[16]={0};
	pid_t pid;




	//check argc
	if(argc==1)
	{
		printf("usage:\n");
		printf("webwrap url=127.0.0.1:8080 prog=/bin/hashseed2tree.exe\n");
	}
	if(argc>15)
	{
		printf("too many args\n");
	}




	//check argv
	for(ii=1;ii<argc;ii++)
	{
		p=argv[ii];
		if(p==0)break;

		//prog=
		if(	(p[0]=='p') &&
			(p[1]=='r') &&
			(p[2]=='o') &&
			(p[3]=='g') &&
			(p[4]=='=') )
		{
			finalargv[0]=p+5;
			finalargv[1]=0;
			break;
		}
		//url=
		if(	(p[0]=='u') &&
			(p[1]=='r') &&
			(p[2]=='l') &&
			(p[3]=='=') )
		{
			url=p+4;	
		}
	}




	//create pipe
	pipe(childear);
	pipe(childmouse);




	//fork
	pid=fork();
	if(pid<0)
	{
		//failed<0
		printf("fork failed\n");
		return;
	}
	else if(pid==0)
	{
		//child=0
		thisischild(2,finalargv);
	}
	else
	{
		//father>0
		thisisfather(url);
	}
}

