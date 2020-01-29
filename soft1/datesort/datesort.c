#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>
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
	while(1)
	{
		if(rsp==0)break;

		if(stack[rsp].folder != 0)closedir(stack[rsp].folder);
		if(stack[rsp].tail !=0)stack[rsp].tail = 0;
		rsp--;
	}

	//convert "/some/dir/" to "/some/dir"
	strncpy(path , p , 1024);
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
#include<stdio.h>
void traverse_write(char*);
char* traverse_read();
char* getdir(char*);
char* getfile(char*);
int wildcard(char*,char*);




int doit(char* file)
{
	char dir[128];
	int ret;
	struct stat st;
	struct tm* ct;
	struct tm* at;
	struct tm* mt;

	ret = stat(file, &st);

	ct = gmtime(&st.st_ctime);
	//printf("ctime: %04d%02d%02d\n", ct->tm_year+1900, ct->tm_mon+1, ct->tm_mday);
	at = gmtime(&st.st_atime);
	//printf("atime: %04d%02d%02d\n", at->tm_year+1900, at->tm_mon+1, at->tm_mday);
	mt = gmtime(&st.st_mtime);
	//printf("mtime: %04d%02d%02d\n", mt->tm_year+1900, mt->tm_mon+1, mt->tm_mday);

	snprintf(dir,128, "%04d%02d%02d", mt->tm_year+1900, mt->tm_mon+1, mt->tm_mday);
	mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);

	printf("mv \"%s\" %s/\n", file, dir);
	return 0;
}
int main(int argc,char** argv)
{
	char* p;
	if(argc==1)return 0;

	if(argc==2)
	{
		traverse_write(argv[1]);
		while(1)
		{
			p=traverse_read();
			if(p==0)break;
			//printf("%s\n", p);

			doit(p);
		}
	}
}
