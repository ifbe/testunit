#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/types.h>
//
#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
//
#define IPADDRESS "127.0.0.1"
#define PORT 2222
#define MAXSIZE 1024
//
void sha1sum(unsigned char* out, const unsigned char* str, int len);
void base64_encode(unsigned char* out,const unsigned char* in,int len);




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
static unsigned char buf[0x100000];
static char* GET = 0;
static char* Connection = 0;
static char* Upgrade = 0;
static char* Sec_WebSocket_Key = 0;
//
static char* http_response = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static int http_response_size;
static char* http_context[0x1000];
static int http_context_size;








void epoll_delete(int fd)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);

	clienttype[fd] = 0;
	close(fd);

	printf("[%d]leave\n\n\n\n\n", fd);
}

void serve_websocket(int fd, int nread)
{
	int i,j,k;
	int type,masked;
        u64 len;
	unsigned char key[4];
        unsigned char buf1[256];
        unsigned char buf2[256];

	for(k=0;k<nread;k++)printf("%.2x ",buf[k]);
	printf("\n");

	//byte0.bit7
	if((buf[0]&0x80)==0x80)printf("tail,");
	else printf("part,");

	//byte0.[3,0]
	k = buf[0]&0xf;
	if(k==0)
	{
		type=0;
		printf("external,");
	}
	else if(k==1)
	{
		type=1;
		printf("text,");
	}
	else if(k==2)
	{
		type=2;
		printf("binary,");
	}
	else if(k==9)
	{
		printf("ping\n");
		return;
	}
	else if(k==0xa)
	{
		printf("pong\n");
		return;
	}
	else if(k==8)
	{
		printf("close\n");
		epoll_delete(fd);
		return;
	}
	else
	{
		printf("known\n");
		epoll_delete(fd);
		return;
	}

	//byte1.bit7
	if( (buf[1]>>7) == 1)
	{
		masked=1;
		printf("masked,");
	}
	else
	{
		masked=0;
		printf("unmasked,");
	}

	//
	k = buf[1]&0x7f;
	if(k==127)
	{
		len     = ((u64)buf[2]<<56)
			+ ((u64)buf[3]<<48)
			+ ((u64)buf[4]<<40)
			+ ((u64)buf[5]<<32)
			+ ((u64)buf[6]<<24)
			+ ((u64)buf[7]<<16)
			+ ((u64)buf[8]<<8)
			+ buf[9];
		k = 10;
		printf("len=%llx,",len);
	}
	else if(k==126)
	{
		len     = (buf[2]<<8)
			+ buf[3];
		k = 4;
		printf("len=%llx,",len);
	}
	else
	{
		len = k;
		k = 2;
		printf("len=%llx,",len);
	}

	if(masked == 1)
	{
		*(u32*)key = *(u32*)(buf + k);
		j = k;
		k += 4;
		printf("key=%x\n",*(u32*)key);

		if(type==1)
		{
			buf[0] &= 0x8f;
			buf[1] &= 0x7f;
			for(i=0;i<len;i++)
			{
				buf[j+i] = buf[i+k] ^ key[i%4];
				printf("%c",buf[j+i]);
			}
			printf("\n");

			write(fd, buf, j+len);
		}
	}
	else printf("\n");
}
void handshake_websocket(int fd)
{
	int k,j,i;
	int type,masked;
	u64 len;
	unsigned char key[4];
	unsigned char buf1[256];
	unsigned char buf2[256];

	j=0;
	while(1)
	{
		if(Sec_WebSocket_Key[j] == 0xa)
		{
			buf1[j] = 0;
			break;
		}
		if(Sec_WebSocket_Key[j] == 0xd)
		{
			buf1[j] = 0;
			break;
		}

		buf1[j] = Sec_WebSocket_Key[j];
		j++;
	}

	snprintf(buf2, 256,"%s%s", buf1, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	printf("%s\n",buf2);

	sha1sum( buf1, buf2, strlen(buf2) );
	for(j=0;j<20;j++)printf("%.2x",buf1[j]);
	printf("\n");

	base64_encode( buf2 ,buf1, 20 );
	printf("%s\n",buf2);

	snprintf(buf, 0x10000,
		"HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: %s\r\n\r\n",

		buf2
	);
	printf("%s",buf);
	write(fd, buf, strlen(buf));
}
void handshake_http(int fd)
{
	int ret;

	ret = write(fd, http_response, http_response_size);
	printf("writing http_response\n");

	ret = write(fd, http_context, http_context_size);
	printf("writing http_context\n");

	epoll_delete(fd);
	printf("\n\n\n\n");
}




static void explainstr(char* buf, int max)
{
	int flag;
	int linehead;

	GET = 0;
	Connection = 0;
	Upgrade = 0;
	Sec_WebSocket_Key = 0;

	linehead = 0;
	while(1)
	{
		if(strncmp(buf+linehead, "GET ", 4) == 0)GET = buf+linehead+4;
		else if(strncmp(buf+linehead, "Connection: ", 12) == 0)Connection = buf+linehead+12;
		else if(strncmp(buf+linehead, "Upgrade: ", 9) == 0)Upgrade = buf+linehead+9;
		else if(strncmp(buf+linehead, "Sec-WebSocket-Key: ", 19) == 0)Sec_WebSocket_Key = buf+linehead+19;

		//eat until next character
		flag=0;
		while(1)
		{
			if(buf[linehead] == 0)
			{
				//printf("[0x0@(%d,%d)]\n",linehead,max);
			}
			else if(buf[linehead] == 0xd)
			{
				flag=1;
				//printf("[0xd@(%d,%d)]\n",linehead,max);
			}
			else if(buf[linehead] == 0xa)
			{
				flag=1;
				//printf("[0xa@(%d,%d)]",linehead,max);
				printf("\n");
			}
			else
			{
				if(flag==0)
				{
					printf("%c", buf[linehead]);
				}
				else break;
			}

			linehead++;
			if(linehead >= max)break;
		}

		if(linehead >= max)break;
	}
	printf("GET@%llx,Connection@%llx,Upgrade@%llx,Sec-WebSocket-Key@%llx\n",
		(u64)GET,
		(u64)Connection,
		(u64)Upgrade,
		(u64)Sec_WebSocket_Key
	);
}
static void do_read(int fd)
{
	int nread;
	struct epoll_event ev;

	nread = read(fd, buf, MAXSIZE);
	if(nread>0)
	{
		buf[nread] = 0;

		//这是个websocket包
		if(clienttype[fd] == 1)
		{
			serve_websocket(fd, nread);
			return;
		}

		//这是个普通socket
		if(clienttype[fd]==0xff)
		{
			//do nothing
			printf("[%d]%s", fd, buf);
			return;
		}

		//头一个包,检查是什么玩意
		else
		{
			//printf("[%d]start {\n", fd);
			//printf("%s", buf);
			//printf("}end [%d]\n\n", fd);
			explainstr(buf, nread);

			//可能是http，websocket
			if(GET != 0)
			{
				//这是个websocket请求
				if( (Upgrade != 0) && (Sec_WebSocket_Key != 0) )
				{
					handshake_websocket(fd);
					clienttype[fd] = 1;
				}

				//http请求根
				else if( (GET[0]=='/')&&(GET[1]==' ') )
				{
					handshake_http(fd);
					return;
				}

				//http请求其他
				else
				{
					epoll_delete(fd);
					return;
				}
			}
			else clienttype[fd] = 0xff;
		}
	}
	else
	{
		if (nread == -1)printf("[%d]read error\n", fd);
		else if (nread == 0)printf("[%d]fd closed\n", fd);
		epoll_delete(fd);
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
		epoll_delete(fd);
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
	struct epoll_event ev;
	struct sockaddr_in cliaddr;

	socklen_t cliaddrlen = sizeof(struct sockaddr_in);
	fd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
	if (fd == -1)printf("accept error\n");

	//
	printf("[%d]%s:%d\n",
		fd,
		inet_ntoa(cliaddr.sin_addr),
		cliaddr.sin_port
	);

	//
	if(fd<clientfirst)clientfirst = fd;
	else if(fd<MAXSIZE)clientlast = fd;
	else
	{
		epoll_delete(fd);
		return;
	}
	clientfd[fd] = 1;

	//
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
static void handle_events()
{
	int i;
	int fd;
	int num = epoll_wait(epollfd, epollevent, MAXSIZE, -1);

	for (i = 0;i < num;i++)
	{
		/*
		if (epollevent[i].events & EPOLLOUT)
		{
			do_write(fd);
		}
		*/

		if (epollevent[i].events & EPOLLIN)
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

	//listen
	listen(listenfd, 5);

	return listenfd;
}

int main(int argc, char *argv[])
{
	int ret;
	struct sigaction sa;
	struct epoll_event ev;

	//clean
	for(ret=0;ret<MAXSIZE;ret++)
	{
		clienttype[ret] = 0;
		clientfd[ret] = 0;
	}

	//http
	ret = open("chat.html",O_RDONLY);
	http_response_size = strlen(http_response);
	http_context_size = read(ret, http_context, 0x1000);
	close(ret);

	//socket
	listenfd = startsocket(IPADDRESS, PORT);

	//epoll
	epollfd = epoll_create(MAXSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//ctrl+c
	signal(SIGINT, stopsocket);

	//epoll.forever
	while(1)
	{
		handle_events();
	}

	//bye
	stopsocket(0);
}

