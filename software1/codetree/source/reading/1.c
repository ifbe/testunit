#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#ifndef O_BINARY
	//mingw64 compatiable
	#define O_BINARY 0x0
#endif




//
void worker_create();
void worker_delete();
void work(char*, u64);
//
int traverse_write();
char* traverse_read();
u64 dealwith(char*);




int learn(int argc,char** argv)
{
	int j;
	char* p;
	u64 x;

	//before
	worker_create();

	//careful, max=20
	for(j=1;j<argc;j++)
	{
		//do it
		traverse_write(argv[j]);
		while(1)
		{
			//get one(traverse.c)
			p = traverse_read();
			if(p == 0)break;
			printf("%s\n",p);

			//find suffix(string.c)
			x = dealwith(p);
			if(x == 0)continue;
			printf("%llx\n",x);

			//do it
			work(p, x);
		}
	}

	//after
	worker_delete();
}
