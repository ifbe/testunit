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
//none(example)
int none_init(void*);
int none_kill();
//count(example)
int count_init(void*);
int count_kill();
//bash
int bash_init(void*);
int bash_kill();
//c
int c_init(void*);
int c_kill();
//cpp
int cpp_init(void*);
int cpp_kill();
//class
int class_init(void*);
int class_kill();
//dts
int dts_init(void*);
int dts_kill();
//go
int go_init(void*);
int go_kill();
//include
int include_init(void*);
int include_kill();
//makefile
int makefile_init(void*);
int makefile_kill();
//java
int java_init(void*);
int java_kill();
//js
int js_init(void*);
int js_kill();
//json
int json_init(void*);
int json_kill();
//perl
int perl_init(void*);
int perl_kill();
//php
int php_init(void*);
int php_kill();
//python
int python_init(void*);
int python_kill();
//ruby
int ruby_init(void*);
int ruby_kill();
//struct
int struct_init(void*);
int struct_kill();




struct worker
{
	u64 id;

	int (*explain)(int,int);
	char pad2[8-sizeof(char*)];

	int (*start)(char*,char*);
	char pad0[8-sizeof(char*)];

	int (*stop)();
	char pad1[8-sizeof(char*)];
};
static struct worker w[20];
static struct stat statbuf;
//
static int infile = -1;
static unsigned char inbuf[0x100000];
static int outfile = -1;
static unsigned char outbuf[0x100000];




//
void work(char* filename,int chosen)
{
	int ret=0;
	int size=0;




//~~~~~~~~~~~~~~~~~~~~~~~1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//stat
	ret=stat( filename , &statbuf );
	if(ret == -1)
	{
		//printf("wrong@stat\n");
		return;
	}

	size = statbuf.st_size;
	if( (size <= 0) | (size > 0xfffff) )
	{
		//printf("wrong@size\n");
		return;
	}

	//infomation
	ret=snprintf(inbuf,256,"#name:       %s\n",filename);
	printf("%s",inbuf);
	//write(outfile,inbuf,ret);

	ret=snprintf(inbuf,256,"#size:       %d(0x%x)\n",size,size);
	printf("%s",inbuf);
	//write(outfile,inbuf,ret);




//~~~~~~~~~~~~~~~~~~~~~~~2~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//open
	infile = open(filename , O_RDONLY|O_BINARY);
	if(infile < 0)
	{
		printf("fail@open\n");
		return;
	}
	//read
	ret=read(infile, inbuf, size);
	if(ret<0)
	{
		printf("fail@read\n");
		close(infile);
		return;
	}

	//close
	inbuf[ret]=0;
	close(infile);




//~~~~~~~~~~~~~~~~~~~~~~~3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//memory
/*
	w[chosen].start(inbuf,outbuf);
	ret = w[chosen].explain(0,size);
	w[chosen].stop();
*/
	c_start(inbuf,outbuf);
	c_explain(0,size);
	c_stop();

	//file
	//write(outfile, outbuf, ret);
}




void worker_create()
{
	char* j = (char*)w;

	none_init(j);	//how many file has been ignored
	j += 0x20;

	count_init(j);	//how many bytes and lines in this file
	j += 0x20;

	c_init(j);
	j += 0x20;

	cpp_init(j);
	j += 0x20;

	dts_init(j);
	j += 0x20;

	include_init(j);
	j += 0x20;

	struct_init(j);
	j += 0x20;

	outfile = open("code.seed", O_WRONLY|O_BINARY);
}
void worker_delete()
{
	if(outfile > 0)close(outfile);
}
