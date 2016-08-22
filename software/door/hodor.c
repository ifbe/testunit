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
static int selffd=0;
static struct sockaddr_in selfbody={0};
//
static int remotefd=0;
static struct sockaddr_in remote;
static socklen_t addrlen;
//
static char buffer[0x100000];
static char* http =
	"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n"
	"<html><body>"
	"<p style=\"text-align:center; font-size:xx-large;\">42</p>"
	"</body></html>";




int startsocket(char* address,int port)
{
	int ret;

	//create struct
	memset(&selfbody, 0, sizeof(struct sockaddr_in));
	selfbody.sin_family=AF_INET;
	selfbody.sin_addr.s_addr=inet_addr(address);
	selfbody.sin_port=htons(port);

	//create socket
	selffd=socket(AF_INET,SOCK_STREAM,0);
	if(selffd==-1)
	{
		printf("socketcreate error\n");
		return -1;
	}

	//
	ret=bind(selffd,(struct sockaddr*)&selfbody,sizeof(struct sockaddr_in));
	if(ret!=0)
	{
		printf("bind error\n");
		return -2;
	}

	//
	ret=listen(selffd,256);
	if(ret!=0)
	{
		printf("listen error\n");
		return -3;
	}

	return 1;
}
static void stopsocket(int num)
{
	close(remotefd);
	close(selffd);
	exit(-1);
}





void main()
{
	int ret;
	struct sigaction sa;

	//
	ret = startsocket("127.0.0.1", 4242);
	if(ret <= 0){printf("error@startsocket:%d\n",ret);return;}

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//ctrl+c
	signal(SIGINT,stopsocket);

	while(1)
	{
		//in
		remotefd = accept(selffd, (struct sockaddr*)&remote, &addrlen);
                if(remotefd<=0)
                {
                        printf("accept error");
                        return;
                }
                printf("in:	%s:%d\n",inet_ntoa(remote.sin_addr),ntohs(remote.sin_port));

		//talk
		while(1)
		{
			ret = read(remotefd, buffer, 1000);
			if(ret <= 0)break;

			buffer[ret]=0;
			printf("%s",buffer);

			if(strncmp(buffer, "GET", 3) == 0)
			{
				ret = write(remotefd, http, strlen(http));
				break;
			}
		}

		//out
                printf("out:	%s:%d\n",inet_ntoa(remote.sin_addr),ntohs(remote.sin_port));
		close(remotefd);
	}
	stopsocket(0);
}
