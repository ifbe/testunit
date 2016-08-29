#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>

#define IPADDRESS   "127.0.0.1"
#define PORT		2222
#define MAXSIZE	 1024
#define LISTENQ	 5
#define FDSIZE	  1000
#define EPOLLEVENTS 100
//
static int epollfd;
static struct epoll_event epollevents[EPOLLEVENTS];
//
static int listenfd;
static int clientfd[16];
//
static char buf[MAXSIZE];







static void do_read(int fd)
{
	int nread;
	struct epoll_event ev;

	nread = read(fd,buf,MAXSIZE);
	if(nread>0)
	{
		printf("[%d]%s", fd, buf);

		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
	}
	else
	{
		if (nread == -1)printf("[%d]error\n", fd);
		else if (nread == 0)printf("[%d]close\n", fd);

		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);

		close(fd);
	}
}

static void do_write(int fd)
{
	int nwrite;
	struct epoll_event ev;

	nwrite = write(fd,buf,strlen(buf));
	if (nwrite == -1)
	{
		perror("write error:");

		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);

		close(fd);
	}
	else
	{
		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
	}
	memset(buf,0,MAXSIZE);
}


static void handle_accpet(int listenfd)
{
	int fd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen = sizeof(struct sockaddr_in);
	fd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if (fd == -1)
	{
		printf("accept error\n");
	}
	else
	{
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd,EPOLL_CTL_ADD,fd, &ev);

		printf("[%d]%s:%d\n",
			fd,
			inet_ntoa(cliaddr.sin_addr),
			cliaddr.sin_port
		);
	}
}
static void handle_events(int num,int listenfd)
{
	int i;
	int fd;
	//进行选好遍历
	for (i = 0;i < num;i++)
	{
		if (epollevents[i].events & EPOLLOUT)
		{
			do_write(fd);
		}
		else if (epollevents[i].events & EPOLLIN)
		{
			fd = epollevents[i].data.fd;
			if(fd == listenfd)
			{
				handle_accpet(listenfd);
			}
			else do_read(fd);
		}
	}
}




static void stopsocket(int num)
{
        close(listenfd);
        exit(-1);
}
static int startsocket(const char* ip,int port)
{
	int reuse=0;
	struct epoll_event ev;
	struct sockaddr_in servaddr;

	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (listenfd == -1)
	{
		perror("socket error:");
		exit(1);
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopet error\n");
		return -1;
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	//bind
	if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
	{
		perror("bind error: ");
		exit(1);
	}
	listen(listenfd,LISTENQ);

	//ctrl+c
	signal(SIGINT,stopsocket);

	//epoll.add
	epollfd = epoll_create(FDSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

	return listenfd;
}

int main(int argc,char *argv[])
{
	int ret;

	//socket
	listenfd = startsocket(IPADDRESS,PORT);

	//epoll.forever
	while(1)
	{
		ret = epoll_wait(epollfd, epollevents, EPOLLEVENTS, -1);
		handle_events(ret,listenfd);
	}

	//bye
	close(epollfd);
	return 0;
}

