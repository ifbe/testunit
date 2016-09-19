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
	unsigned char name[512 - sizeof(char*)];
};
static struct stack stack[16];
//
static struct stat statbuf;
//
static char path[512];
//
static int rsp=0;




char* traverse_read()
{
	struct dirent* ent;

	while(1)
	{
		//empty name
		if(stack[rsp].name[0] == 0)
		{
			//empty stack
			if(rsp == 0)return 0;

			//pop
			else
			{
				rsp--;
				continue;
			}
		}

		//have name, not opened
		if(stack[rsp].folder == 0)
		{
			//try to open dir
			stack[rsp].folder = opendir(stack[rsp].name);

			//opened successfully
			if(stack[rsp].folder != 0)continue;

			//can not open, it is leaf !!!
			else
			{
				strncpy(path,stack[rsp].name,500);
				stack[rsp].name[0]=0;
				return path;
			}
		}

		//folder opened, take one
		ent=readdir(stack[rsp].folder);

		//failed to get
		if(ent == 0)
		{
			closedir(stack[rsp].folder);
			stack[rsp].name[0]=0;

			if(rsp > 0)rsp--;
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
		if(	(rsp==0) &&
			(stack[0].name[0]=='/') &&
			(stack[0].name[1]==0) )
		{
			snprintf(
				stack[1].name,
				500,
				"/%s",
				ent->d_name
			);
		}
		else
		{
			snprintf(
				stack[rsp+1].name,
				500,
				"%s/%s",
				stack[rsp].name,
				ent->d_name
			);
		}
		stack[rsp+1].folder = 0;
		rsp++;
	}
}
int traverse_write(char* p)
{
	int j;

	//clear everything
	rsp=0;
	memset(stack,0,sizeof(struct stack));
	strncpy(stack[0].name , p , 256);

	//convert "/some/dir/" to "/some/dir"
	j=strlen(p);
	if(j > 1)
	{
		if(p[j-1]=='/')stack[0].name[j-1]=0;
	}
}
