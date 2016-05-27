#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define serveraddr "10.42.0.2"
#define serverport 8888

//static char buf[100] ={'L','R',0,0};
static char buf[100] ={'X','Y',0,0};



int main()
{
        int ret = 0;
	int client;
        struct sockaddr_in ser_addr = {0};

	/*定义服务器端地址*/
	memset(&ser_addr, 0, sizeof(struct sockaddr_in));
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_port = htons(serverport);
        ser_addr.sin_addr.s_addr = inet_addr(serveraddr);

	/*创建socket*/
        client = socket(AF_INET, SOCK_STREAM, 0);
        if (client == -1)
        {
                printf("socket create error\n");
		exit(-1);
        }
		
	/*连接服务器*/
        ret = connect(client, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
        if (ret == -1)
        {
                printf("socket connect error\n");
		exit(-1);
        }
		
	/*通信*/
        ret = write(client, buf, 16);
        if(ret <= 0)
        {
                printf("send error");
		exit(-1);
        }

	int l,r;
	while(1)
	{
		scanf("%d %d",&l,&r);
		*(int*)(buf+4)=l;
		*(int*)(buf+8)=r;
		write(client,buf,16);
	}

	/*退出*/
        close(client);
        return 0;
}
