#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<signal.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
//
static int childear[2];
static int childmouth[2];
//
static int position=0;
static unsigned char haha[0x100000*16]={0};
//
static unsigned char httpbuf[0x1000];
static unsigned char http_response[]="HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static unsigned char http_head[0x1000] = {0};
static unsigned char http_tail[] = "</pre></body></html>";




int initmywebserver(char* url)
{
	int ret;
	int sockfd;
	int serverport=8080;
	struct sockaddr_in addr;
	struct sigaction sa;

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
	addr.sin_port = htons(serverport);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
	{
		perror("socket binding failed!\n");
		exit(-1);
	}
	listen(sockfd, 128);

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//everything ok
	return sockfd;
}
void thisisfather(char* url)
{
	//temp
	int ret;
	int failcount;
	//socket related
	int sockfd;
	int thisfd;




	//1.检查传进来的参数
	if(url==0)
	{
		snprintf(url,256,"http://127.0.0.1:8080");
	}
	snprintf(
		http_head,
		0x1000,

		"<html><head><title>codetree</title></head><body>"
		"<form method=\"get\" action=\"%s\" style=\"text-align:center;\">"
		"<input type=\"text\" name=\"i\" style=\"width:40%;height:32px\"></input>"
		"<input type=\"submit\" style=\"width:10%;height:32px\"></input>"
		"</form><hr><pre>",

		url
	);
	sockfd=initmywebserver(url);
	if(sockfd<=0)exit(-1);




	//2.设置不阻塞，分别关掉两个单向管道的某一边
	//non block
	ret=fcntl(childmouth[0], F_GETFL);
	fcntl(childmouth[0],F_SETFL,ret|O_NONBLOCK);

	//parent only write to child's ear
	close(childear[0]);

	//parent only read from child's mouth
	close(childmouth[1]);




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
			ret=read(childmouth[0],haha+position,0x1000);
			if(ret<0)
			{
				//printf("ret=%d,errno=%d\n",ret,errno);
				usleep(1000);
				fsync(childmouth[0]);

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
		ret=write(thisfd, http_response, strlen(http_response));
		if(ret<0)goto nextone;

		ret=write(thisfd, http_head, strlen(http_head));
		if(ret<0)goto nextone;

		//身体
		if(position!=0)
		{
			ret=write(thisfd, haha, position);
			if(ret<0)goto nextone;
		}

		//尾巴
		ret=write(thisfd, http_tail, strlen(http_tail));
		if(ret<0)goto nextone;

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
	dup2(childmouth[1],1);
	close(childmouth[0]);

	//bye bye
	ret=execvp(argv[0],argv);
	if(ret==-1)
	{
		//printf("execv fail:%d\n",errno);
		close(childear[0]);
		close(childmouth[1]);
		exit(-1);
	}
}








void buf2arg(char* buf,int max,int* argc,char** argv)
{
	int i;
	int count=0;
	int splited=0;
	argv[0]=0;

	//
	for(i=0;i<max;i++)
	{
		//finished
		if( buf[i] == 0 )break;

		//blank
		if( buf[i] <= 0x20 )
		{
			buf[i]=0;
			splited=1;
			continue;
		}

		//new?
		if(splited != 0)
		{
			count++;
			if(count>=7)break;

			argv[count]=0;
			splited=0;
		}

		//new!
		if( argv[count]==0 )
		{
			argv[count]=buf+i;
		}
	}//for

	//result
	count+=1;
	argv[count]=0;
	*argc=count;

/*
	//debug
	say("count=%x\n",count);
	for(i=0;i<count;i++)
	{
		say("%x=%s\n",i,argv[i]);
	}
*/
}
void main(int argc, char *argv[])
{
	//
	int ii;
	char* p;
	char* url=0;
	//
	int finalargc;
	char* finalargv[16]={0};
	//
	pid_t pid;
	char temp[256];




	//check argc
	if(argc==1)
	{
		printf("usage:\n");
		printf("webwrap url=127.0.0.1:8080 prog=/bin/hashseed2tree.exe\n");
		return;
	}
	if(argc>15)
	{
		printf("too many args\n");
		return;
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
			snprintf(temp,256,"%s",p+5);
			buf2arg(temp,256,&finalargc,finalargv);
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
	if(url==0)
	{
		printf("wrong url\n");
		return;
	}
	if(finalargv[0]==0)
	{
		printf("wrong prog\n");
		return;
	}




	//create pipe
	pipe(childear);
	pipe(childmouth);




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
		thisischild(finalargc,finalargv);
	}
	else
	{
		//father>0
		thisisfather(url);
	}
}

