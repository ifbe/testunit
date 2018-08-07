#define _XOPEN_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
#include<termios.h>
#include<pthread.h>
#include<sys/ioctl.h>
int grantpt(int);
int unlockpt(int);
char* ptsname(int);




static int master;
static int slave;
static void sig_int(int a)
{
	char ch[1] = {0x3};
	write(master, ch, 1);
}
static void sig_tstp(int a)
{
	char ch[1] = {0x1a};
	write(master, ch, 1);
}
void childprocess(char* name)
{
	int ret;
	slave = open(name, O_RDWR);
	if(slave == 0)
	{
		printf("error@open:%d\n",errno);
		return;
	}
	ret = ioctl(slave, TIOCSCTTY, NULL);
	//if(ret == -1)
	//{
	//	printf("error@ioctl:%d\n",errno);
	//}

	setsid();
	dup2(slave, 0);
	dup2(slave, 1);
	dup2(slave, 2);
	execl("/bin/bash", "/bin/bash", NULL);
}
void* readthread(void* ignore)
{
	int ret;
	char buf[0x1000];
	while(1)
	{
		ret = read(master,buf,0x1000);
		if(ret <= 0)break;

		printf("%.*s",ret,buf);
	}
	master = 0;
}
void main()
{
	int ret;
	char* name;
	pthread_t thread;

	master = open("/dev/ptmx", O_RDWR);
	if(master <= 0)
	{
		printf("error@open:%d\n",errno);
		return;
	}

	ret = grantpt(master);
	if(ret < 0)
	{
		printf("error@grantpt:%d\n",errno);
		return;
	}

	ret = unlockpt(master);
	if(ret < 0)
	{
		printf("error@unlockpt:%d\n",errno);
		return;
	}

	name = ptsname(master);
	if(name == 0)
	{
		printf("error@ptsname:%d\n",errno);
		return;
	}
	printf("%.*s\n", 16, name);

	ret = fork();
	if(ret < 0)return;
	else if(ret == 0)childprocess(name);
	else
	{
		signal(SIGINT, sig_int);
		signal(SIGTSTP, sig_tstp);
		signal(SIGCHLD, SIG_IGN);
		pthread_create(&thread, 0, readthread, 0);
		while(1)
		{
			if(master == 0)break;
			ret = getchar();
			if(ret > 0)write(master, &ret, 1);
		}
	}
}
