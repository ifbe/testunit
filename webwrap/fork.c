#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
static int fd1[2];
static int fd2[2];




void thisisfather()
{
	while(1)
	{
		printf("father=%d\n",getpid());
		sleep(1);
	}
}
void thisischild()
{
	int ret;
	char* arg[2]={"/opt/bin/hashseed2tree.exe",0};

	//redirect stdin,stdout
	dup2(fd1[0],0);
	dup2(fd2[1],1);

	//bye bye
	ret=execvp(arg[0],arg);
	if(ret==-1)
	{
		printf("execv fail:%d\n",errno);
	}
}
void main(int argc, char *argv[ ], char **env)
{
        pid_t pid;
	pipe(fd1);
	pipe(fd2);
        pid =fork();

	//failed<0
        if(pid<0)
	{
		printf("fork failed\n");
		return;
	}

	//child=0
	else if(pid==0)
        {
		thisischild();
        }

	//father>0
        else
        {
		thisisfather();
        }
}

