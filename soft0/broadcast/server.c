#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
 
 
int main()
{
	// 绑定地址
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(6000);
	
	// 广播地址
	struct sockaddr_in from;
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(6000);
	
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		printf("socket error\n");	
		return -1;
	}   
 
	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
	{   
		printf("bind error...\n");
		return -1;
	}
 
	int len = sizeof(struct sockaddr_in);
	char smsg[100] = {0};
 
	while(1)
	{
		//从广播地址接受消息
		int ret=recvfrom(sock, smsg, 100, 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if(ret<=0)
		{
			printf("read error....\n");
		}
		else
		{		
			printf("%s\n",smsg);	
		}
 
		sleep(1);
	}
 
	return 0;
}
