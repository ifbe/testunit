#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef O_BINARY
	//mingw64 compatiable
	#define O_BINARY 0x0
#endif




static ignorecount;
int none_explain(int start,int end)
{
	return 0;
}
int none_start(char* thisfile,int size)
{
	ignorecount++;
	return 0;
}
int none_stop(int where)
{
	return 0;
}
int none_init(char* file,char* memory)
{
	ignorecount=0;
	return 0;
}
int none_kill()
{
	printf("ignored %d files\n",ignorecount);
	return 0;
}
