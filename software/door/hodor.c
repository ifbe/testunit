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
#define u64 unsigned long long
#define u32 unsigned int
void sha1sum(unsigned char* out, const unsigned char* str, int len);
void base64_encode(unsigned char* out,const unsigned char* in,int len);




//
static int listenfd=0;
static struct sockaddr_in selfbody={0};
//
static int clientfd=0;
static struct sockaddr_in clientaddr;
static socklen_t addrlen;
//
static char* http_response = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static int http_response_size;
static char* http_context[0x1000];
static int http_context_size;
//
static unsigned char buffer[0x100000];
static char* GET = 0;
static char* Connection = 0;
static char* Upgrade = 0;
static char* Sec_WebSocket_Key = 0;




int startsocket(char* address,int port)
{
	int ret;
	int reuse=0;

	//create socket
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1)
	{
		printf("socketcreate error\n");
		return -1;
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopet error\n");
		return -1;
	}

	//create struct
	memset(&selfbody, 0, sizeof(struct sockaddr_in));
	selfbody.sin_family=AF_INET;
	selfbody.sin_addr.s_addr=inet_addr(address);
	selfbody.sin_port=htons(port);

	//
	ret=bind(listenfd,(struct sockaddr*)&selfbody,sizeof(struct sockaddr_in));
	if(ret!=0)
	{
		printf("bind error\n");
		return -2;
	}

	//
	ret=listen(listenfd,256);
	if(ret!=0)
	{
		printf("listen error\n");
		return -3;
	}

	return 1;
}
static void stopsocket(int num)
{
	close(clientfd);
	close(listenfd);
	exit(-1);
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
			else if(buf[linehead] == 0xa)
			{
				flag=1;
				//printf("[0xa@(%d,%d)]",linehead,max);
				printf("\n");
			}
			else if(buf[linehead] == 0xd)
			{
				flag=1;
				//printf("[0xd@(%d,%d)]\n",linehead,max);
			}
			else
			{
				if(flag==0)printf("%c", buf[linehead]);
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
void serve_websocket(int fd)
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

	snprintf(buffer, 0x10000,
		"HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: %s\r\n\r\n",

		buf2
	);
	printf("%s",buffer);

	j = write(fd, buffer, strlen(buffer));

	while(1)
	{
		j = read(fd, buffer, 1000);
		if(j <= 0)return;

		for(k=0;k<j;k++)printf("%.2x ",buffer[k]);
		printf("\n");

		//byte0.bit7
		if((buffer[0]&0x80)==0x80)printf("tail,");
		else printf("part,");

		//byte0.[3,0]
		k = buffer[0]&0xf;
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
			continue;
		}
		else if(k==0xa)
		{
			printf("pong\n");
			continue;
		}
		else if(k==8)
		{
			printf("close\n");
			return;
		}
		else
		{
			printf("known\n");
			return;
		}

		//byte1.bit7
		if( (buffer[1]>>7) == 1)
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
		k = buffer[1]&0x7f;
		if(k==127)
		{
			len	= ((u64)buffer[2]<<56)
				+ ((u64)buffer[3]<<48)
				+ ((u64)buffer[4]<<40)
				+ ((u64)buffer[5]<<32)
				+ ((u64)buffer[6]<<24)
				+ ((u64)buffer[7]<<16)
				+ ((u64)buffer[8]<<8)
				+ buffer[9];
			k = 10;
			printf("len=%llx,",len);
		}
		else if(k==126)
		{
			len	= (buffer[2]<<8)
				+ buffer[3];
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
			*(u32*)key = *(u32*)(buffer + k);
			j = k;
			k += 4;
			printf("key=%x\n",*(u32*)key);

			if(type==1)
			{
				buffer[0] &= 0x8f;
				buffer[1] &= 0x7f;
				for(i=0;i<len;i++)
				{
					buffer[j+i] = buffer[i+k] ^ key[i%4];
					printf("%c",buffer[j+i]);
				}
				printf("\n");

				write(fd, buffer, j+len);
			}
		}
		else printf("\n");

	}//while1
}
void serve_http(int fd)
{
	int ret;

	ret = write(fd, http_response, http_response_size);
	printf("writing http_response\n");

	ret = write(fd, http_context, http_context_size);
	printf("writing http_context\n");
}
void main()
{
	int ret;
	struct sigaction sa;

	//
	ret = open("chat.html",O_RDONLY);
	http_response_size = strlen(http_response);
	http_context_size = read(ret, http_context, 0x1000);
	close(ret);

	//
	ret = startsocket("0.0.0.0", 2222);
	if(ret <= 0){printf("error@startsocket:%d\n",ret);return;}

	//do not stop when SIGPIPE
	sa.sa_handler=SIG_IGN;
	sigaction(SIGPIPE,&sa,0);

	//ctrl+c
	signal(SIGINT,stopsocket);

	while(1)
	{
		//in
		addrlen = sizeof(struct sockaddr_in);
		clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &addrlen);
		if(clientfd<=0)
		{
			printf("accept error\n");
			return;
		}
		printf("%s:%d {\n",inet_ntoa(clientaddr.sin_addr),clientaddr.sin_port);

		//talk
		while(1)
		{
			ret = read(clientfd, buffer, 1000);
			if(ret <= 0)break;

			buffer[ret]=0;
			explainstr(buffer, ret);
			if(GET != 0)
			{
				if( (Upgrade != 0) && (Sec_WebSocket_Key != 0) )
				{
					serve_websocket(clientfd);
				}
				else if( (GET[0]=='/')&&(GET[1]==' ') )
				{
					serve_http(clientfd);
				}
			}

			break;
		}

		//out
		printf("} %s:%d\n\n\n\n\n",
			inet_ntoa(clientaddr.sin_addr),
			clientaddr.sin_port
		);
		close(clientfd);
	}
	stopsocket(0);
}
