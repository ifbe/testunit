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
#define SERVER_PORT 8080
//
const static char http_response[] =
"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_head[] =
"<html><head><title>codetree</title></head><body>"
"<form method=\"get\" action=\"http://127.0.0.1:8080\" style=\"text-align:center;\">"
"<input type=\"text\" name=\"i\" style=\"width:40%;height:32px\"></input>"
"<input type=\"submit\" style=\"width:10%;height:32px\"></input>"
"</form><hr><pre>";
const static char http_tail[] =
"</pre></body></html>";
static char httpbuf[0x1000];
static int sockfd;
//
static int childear[2];
static int childmouse[2];
unsigned char haha[0x100000]={0};
int position=0;




void initmywebserver()
{
	struct sockaddr_in addr;

	//建立TCP套接字   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket creation failed!\n");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	//这里要注意，端口号一定要使用htons先转化为网络字节序，否则绑定的实际端口  
	//可能和你需要的不同   
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
	{
		perror("socket binding failed!\n");
		exit(-1);
	}
	listen(sockfd, 128);
}
void thisisfather(char** arg)
{
	//local
	int ret;
	int failcount;

	//web
	int thisfd;
	char* getname;
	char* getsize;

	//non block
	ret=fcntl(childmouse[0], F_GETFL);
	fcntl(childmouse[0],F_SETFL,ret|O_NONBLOCK);

	//parent only write to child's ear
	close(childear[0]);
	//parent only read from child's mouse
	close(childmouse[1]);

	//
	initmywebserver();

	//start!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while(1)
	{
        	thisfd = accept(sockfd, NULL, NULL);
		read(thisfd, httpbuf, 1024);
//printf("1\n");
		//其他HTTP请求处理，如POST，HEAD等 。这里我们只处理GET   
		if(strncmp(httpbuf, "GET", 3) != 0)
		{
		        printf("unsupported request!\n");
		        goto nextone;
		}

//printf("2\n");
		//是GET请求
		getname = httpbuf + 4;
		getsize = strchr(getname, ' ');
		*getsize = '\0';

		//发给exe，把拿到的回复扔回去
		if(strncmp(getname,"/?i=",4)!=0)
		{
			position=0;
			goto sendresponse;
		}

//printf("name=%s,size=%d\n",getname,getsize-getname);
		//发给exe
		ret=snprintf(haha,256,"%s\n",getname+4);
		ret=write(childear[1],haha,ret);
		if(ret<=0)
		{
			printf("pipe closed\n");
			exit(-1);
		}

//printf("4\n");
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
		write(thisfd, http_response, strlen(http_response));
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
void thisischild(char** arg)
{
	int ret;
	//char* thisarg[2]={"/opt/bin/hashseed2tree.exe",0};

	//子进程只输入，不输出
	dup2(childear[0],0);
	close(childear[1]);

	//子进程只输出，不输入
	dup2(childmouse[1],1);
	close(childmouse[0]);

	//bye bye
	//ret=execvp(thisarg[0],thisarg);
	arg++;
	ret=execvp(arg[0],arg);
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
        pid_t pid;

	//check
	if(argv[1]==0)
	{
		printf("wrong exe\n");
		exit(-1);
	}

	//create pipe
	pipe(childear);
	pipe(childmouse);
        pid =fork();

	//failed<0
        if(pid<0)
	{
		printf("fork failed\n");
		return;
	}

	//child=0
	else if(pid==0)
        {
		thisischild(argv);
        }

	//father>0
        else
        {
		thisisfather(argv);
        }
}

