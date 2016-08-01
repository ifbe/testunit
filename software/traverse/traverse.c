#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
struct stack
{
	DIR* folder;
	char* tail;
};
static struct stack stack[32];		//16 is not enough
//
static char path[0x1000];		//some very long
//
static struct stat statbuf;
//
static int rsp=0;




char* traverse_read()
{
	struct dirent* ent;

	while(1)
	{
		//empty name
		if(stack[rsp].tail == 0)
		{
			//empty stack
			if(rsp == 0)
			{
				return 0;
			}

			//stack pop, new name
			else
			{
				rsp--;
				stack[rsp].tail[0]=0;
				continue;
			}
		}

		//have name, not opened
		if(stack[rsp].folder == 0)
		{
			//try to open dir
			if(path[0]==0)stack[rsp].folder = opendir("/");
			else stack[rsp].folder = opendir(path);

			//opened successfully
			if(stack[rsp].folder != 0)continue;

			//can not open, it is leaf !!!
			else
			{
				stack[rsp].tail=0;
				return path;
			}
		}

		//folder opened, take one
		ent=readdir(stack[rsp].folder);

		//nothing left in this folder
		if(ent == 0)
		{
			closedir(stack[rsp].folder);
			stack[rsp].tail=0;

			continue;
		}

		//kill nondir
		if(ent->d_type == DT_LNK)
		{
			continue;
		}

		//ignore . .. .*
		if(ent->d_name[0] == '.')
		{
			continue;
		}

		//push
		stack[rsp+1].folder = 0;
		stack[rsp+1].tail = stack[rsp].tail + snprintf(
			stack[rsp].tail,
			1024,
			"/%s",
			ent->d_name
		);
		rsp++;
	}
}
void traverse_write(char* p)
{
	int j;
	if(p[0]==0)return;

	//clear everything
	rsp=0;
	strncpy(path , p , 1024);

	//convert "/some/dir/" to "/some/dir"
	j=strlen(p);
	if(p[j-1]=='/')
	{
		path[j-1]=0;
		stack[0].folder=0;
		stack[0].tail=path+j-1;
	}
	else
	{
		stack[0].folder=0;
		stack[0].tail=path+j;
	}
}
