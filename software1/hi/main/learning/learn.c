#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#ifndef O_BINARY
        //mingw64 compatiable
        #define O_BINARY 0x0
#endif




//none(example)
int none_init(char*,char*);
int none_kill();
//count(example)
int count_init(char*,char*);
int count_kill();
//bash
int bash_init(char*,char*);
int bash_kill();
//c
int c_init(char*,char*);
int c_kill();
//cpp
int cpp_init(char*,char*);
int cpp_kill();
//class
int class_init(char*,char*);
int class_kill();
//dts
int dts_init(char*,char*);
int dts_kill();
//go
int go_init(char*,char*);
int go_kill();
//include
int include_init(char*,char*);
int include_kill();
//makefile
int makefile_init(char*,char*);
int makefile_kill();
//java
int java_init(char*,char*);
int java_kill();
//js
int js_init(char*,char*);
int js_kill();
//json
int json_init(char*,char*);
int json_kill();
//perl
int perl_init(char*,char*);
int perl_kill();
//php
int php_init(char*,char*);
int php_kill();
//python
int python_init(char*,char*);
int python_kill();
//ruby
int ruby_init(char*,char*);
int ruby_kill();
//struct
int struct_init(char*,char*);
int struct_kill();
//
int dealwith(char*);
int traverse_write();
char* traverse_read();




struct worker
{
	//[0,f]
	char suffix[8];
	char worker[8];

	//[10,1f]
	int (*start)();
	char pad0[8-sizeof(char*)];
	int (*stop)();
	char pad1[8-sizeof(char*)];

	//[20,2f]
	int (*explain)(int,int);
	char pad2[8-sizeof(char*)];
	int (*what)(int,int);
	char pad3[8-sizeof(char*)];

	//[30,3f]
	int (*haha)(int,int);
	char pad4[8-sizeof(char*)];
	int (*hehe)(int,int);
	char pad5[8-sizeof(char*)];
};
static struct worker* worker;


//
static unsigned char* inbuffer;
static unsigned char* outbuffer;




//
void work(char* filename,int size)
{
	int ret=0;
	int chosen=0;
	int targetfd=-1;

	//empty file
	if(size == 0)return;
	if(size>0x100000)return;

	//open
	targetfd=open(filename , O_RDONLY|O_BINARY);
	if(targetfd<0)
	{
		printf("fail@open\n");
		return;
	}

	//read
	ret=read(targetfd,inbuffer,size);
	if(ret<0)
	{
		printf("fail@read\n");
		goto finish;
	}
	inbuffer[ret]=0;

	//who can fight this
	chosen = dealwith(filename);
	if(chosen < 0)return;

	//start executing
	worker[chosen].start();
	worker[chosen].explain(0,size);
	worker[chosen].stop();

finish:
	close(targetfd);
	return;
}




int learn(int argc,char** argv)
{
/*
        //special:
        none_init(w,i);  //how many file has been ignored
        w+=0x40;

        count_init(w,i); //how many bytes and lines in this file
        w+=0x40;

        //normal:
        c_init(w,i);
        w+=0x40;

        cpp_init(w,i);
        w+=0x40;

        dts_init(w,i);
        w+=0x40;

        include_init(w,i);
        w+=0x40;

        struct_init(w,i);
        w+=0x40;

        //careful, max=20
*/
	int j;
	for(j=1;j<argc;j++)
	{
		printf("%s\n",argv[j]);
	}

	char* p;
	traverse_write(argv[1]);
	while(1)
	{
	        p=traverse_read();
	        if(p==0)break;

	        printf("%s\n",p);
	}
}
