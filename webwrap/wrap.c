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
static int childearparentmouse[2];
static int childmouseparentear[2];




char haha[256]={0};
void thisisfather()
{
	int ret;
	ret=fcntl(childmouseparentear[0], F_GETFL);
	fcntl(childmouseparentear[0],F_SETFL,ret|O_NONBLOCK);

	while(1)
	{
		//wait from stdin,write to that program
		fgets(haha,100,stdin);
		write(childearparentmouse[1],haha,strlen(haha));

		//read from that program,write to stdout
		ret=0;
		while(1)
		{
			ret=read(childmouseparentear[0],haha,100);
			if(ret<=0)break;

			haha[ret]=0;
			printf("%s\n",haha);
		}
	}
}
void thisischild()
{
	int ret;
	char* arg[2]={"/opt/bin/hashseed2tree.exe",0};

	//子进程只输入，不输出
	dup2(childearparentmouse[0],0);
	close(childearparentmouse[1]);

	//子进程只输出，不输入
	dup2(childmouseparentear[1],1);
	close(childmouseparentear[0]);

	//bye bye
	ret=execvp(arg[0],arg);
	if(ret==-1)
	{
		printf("execv fail:%d\n",errno);
	}
}
void main(int argc, char *argv[])
{
        pid_t pid;
	pipe(childearparentmouse);
	pipe(childmouseparentear);
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

