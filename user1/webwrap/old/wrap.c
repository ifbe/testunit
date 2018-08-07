#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<sys/stat.h>
static int childear[2];
static int childmouse[2];




unsigned char haha[0x100000]={0};
int position=0;
void thisisfather(char** arg)
{
	int ret;
	int failcount;

	//non block
	ret=fcntl(childmouse[0], F_GETFL);
	fcntl(childmouse[0],F_SETFL,ret|O_NONBLOCK);

	//parent only write to child's ear
	close(childear[0]);
	//parent only read from child's mouse
	close(childmouse[1]);

	while(1)
	{
		//wait from stdin,write to that program
		fgets(haha,100,stdin);
		//printf("len=%d,name=%s\n",strlen(haha),haha);

		ret=write(childear[1],haha,strlen(haha));
		if(ret<=0)
		{
			printf("pipe closed\n");
			exit(-1);
		}

                //read from that program,write to stdout
                ret=0;
		failcount=0;
                position=0;
                while(1)
                {
                        ret=read(childmouse[0],haha+position,0x1000);
                        if(ret<0)
			{
				//printf("ret=%d,errno=%d\n",ret,errno);
				usleep(1000);
				fsync(childmouse[0]);

				failcount++;
				if(failcount<100)continue;
				else break;
			}
			else if(ret==0)
			{
				printf("pipe closed\n");
				exit(-1);
			}

                        position+=ret;
			if(position>1000000)break;
                }
                haha[position]=0;
                printf("%s",haha,position);
	}
}
void thisischild(char** arg)
{
	int ret;
	//char* thisarg[2]={"/opt/bin/hashseed2tree.exe",0};

	//子进程只输入，不输出
	dup2(childear[0],0);
	close(childear[1]);

	//子进程只输出，不输入
	dup2(childmouse[1],1);
	close(childmouse[0]);

	//bye bye
	//ret=execvp(thisarg[0],thisarg);
	arg++;
	ret=execvp(arg[0],arg);
	if(ret==-1)
	{
		//printf("execv fail:%d\n",errno);
		close(childear[0]);
		close(childmouse[1]);
		exit(-1);
	}
}
void main(int argc, char *argv[])
{
        pid_t pid;

	//check
	if(argv[1]==0)
	{
		printf("wrong exe\n");
		exit(-1);
	}

	//create pipe
	pipe(childear);
	pipe(childmouse);
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
		thisischild(argv);
        }

	//father>0
        else
        {
		thisisfather(argv);
        }
}

