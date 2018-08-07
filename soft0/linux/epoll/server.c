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

#define IPADDRESS "127.0.0.1"
#define PORT 2222
#define MAXSIZE 1024
//
static int listenfd;
static int epollfd;
static struct epoll_event epollevent[MAXSIZE];
//
static int clientfirst=0;
static int clientlast=0;
static int clienttype[MAXSIZE];
static int clientfd[MAXSIZE];
//
static char buf[0x100000];







static void do_read(int fd)
{
	int nread;
	struct epoll_event ev;

	nread = read(fd, buf, MAXSIZE);
	if(nread>0)
	{
		printf("[%d]start {\n", fd);
		printf("%s", buf);
		printf("}end [%d]\n\n", fd);

		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
	}
	else
	{
		if (nread == -1)printf("[%d]read error\n", fd);
		else if (nread == 0)printf("[%d]fd closed\n", fd);

		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);

		close(fd);
	}
}

static void do_write(int fd)
{
	int nwrite;
	struct epoll_event ev;

	nwrite = write(fd, buf, strlen(buf));
	if (nwrite == -1)
	{
		printf("[%d]write error\n", fd);

		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);

		close(fd);
	}
	else
	{
		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
	}
	memset(buf, 0, MAXSIZE);
}


static void handle_accpet(int listenfd)
{
	int fd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen = sizeof(struct sockaddr_in);
	fd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
	if (fd == -1)
	{
		printf("accept error\n");
	}
	else
	{
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = fd;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);

		printf("[%d]%s:%d\n",
			fd,
			inet_ntoa(cliaddr.sin_addr),
			cliaddr.sin_port
		);

		if(fd<clientfirst)clientfirst = fd;
		else clientlast = fd;
		clientfd[fd] = 1;
	}
}
static void handle_events()
{
	int i;
	int fd;
	int num = epoll_wait(epollfd, epollevent, MAXSIZE, -1);

	for (i = 0;i < num;i++)
	{
		if (epollevent[i].events & EPOLLOUT)
		{
			do_write(fd);
		}
		else if (epollevent[i].events & EPOLLIN)
		{
			fd = epollevent[i].data.fd;

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
	close(epollfd);
        exit(-1);
}
static int startsocket(const char* ip, int port)
{
	int reuse=0;
	struct epoll_event ev;
	struct sockaddr_in servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		printf("socket error\n");
		exit(1);
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		printf("setsockopet error\n");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	//bind
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		printf("bind error\n");
		exit(1);
	}
	listen(listenfd, 5);

	//ctrl+c
	signal(SIGINT, stopsocket);

	//epoll.add
	epollfd = epoll_create(MAXSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

	return listenfd;
}

int main(int argc, char *argv[])
{
	int ret;
	for(ret=0;ret<MAXSIZE;ret++)
	{
		clienttype[ret] = 0;
		clientfd[ret] = 0;
	}

	//socket
	listenfd = startsocket(IPADDRESS, PORT);

	//epoll.forever
	while(1)
	{
		handle_events();
	}

	//bye
	stopsocket(0);
}

