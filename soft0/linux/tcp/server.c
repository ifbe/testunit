#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<signal.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
#define serverport 8888
unsigned int buffer[64];




int main()
{
	int ret;
	int sockfd;
	int thisfd;
	struct sigaction sa;
	struct sockaddr_in self;
	struct sockaddr_in remote;
	socklen_t addrlen;

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//init
	memset(&self, 0, sizeof(struct sockaddr_in));
	self.sin_family = AF_INET;
	self.sin_port = htons(serverport);
	self.sin_addr.s_addr = INADDR_ANY;

	//建立TCP套接字   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket creation failed!\n");
		exit(-1);
	}

	//bind,listen
	ret=bind(sockfd, (struct sockaddr *)&self, sizeof(struct sockaddr_in));
	if(ret!=0)
	{
		perror("socket binding failed!\n");
		exit(-1);
	}
	listen(sockfd, 128);

	//3.开始!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while(1)
	{
		thisfd = accept(sockfd, (struct sockaddr*)&remote, &addrlen);
		printf("%s:%d\n",inet_ntoa(remote.sin_addr),ntohs(remote.sin_port));

		while(1)
		{
			ret=read(thisfd, buffer, 1024);
			if(ret<=0)
			{
				close(thisfd);
				break;
			}
			printf("%x,%x,%x,%x\n",buffer[0],buffer[1],buffer[2],buffer[3]);

			//头
			ret=write(thisfd, "haha\n", 5);
			if(ret<0)
			{
				close(thisfd);
				break;
			}
		}//talk to the connected
	}//wait for one connection
}
