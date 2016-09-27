#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
//
static int socketfd=0;
static struct sockaddr_in selfbody={0};
//
static char buffer[0x100000];




int startsocket(char* address,int port)
{
	int ret;

	//create struct
	memset(&selfbody, 0, sizeof(struct sockaddr_in));
	selfbody.sin_family=AF_INET;
	selfbody.sin_addr.s_addr=inet_addr(address);
	selfbody.sin_port=htons(port);

	//create socket
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd==-1)
	{
		//printf("socketcreate error\n");
		return -1;
	}

	//
	ret=connect(socketfd,(struct sockaddr*)&selfbody,sizeof(selfbody));
	if(ret<0)
	{
		//printf("connect error\n");
		return 0;
	}

	return 1;
}
static void stopsocket(int num)
{
	close(socketfd);
	exit(-1);
}





void main(int argc, char** argv)
{
	int ret;
	struct sigaction sa;

	//
	ret = startsocket("127.0.0.1", 42);
	if(ret <= 0){printf("error@startsocket:%d\n",ret);return;}

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//ctrl+c
	signal(SIGINT,stopsocket);

	while(1)
	{
		ret = write(socketfd, "hello\n", 6);
		if(ret <= 0)break;

		sleep(1);
	}
	stopsocket(0);
}
