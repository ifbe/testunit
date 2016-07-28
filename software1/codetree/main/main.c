#include <stdio.h>
#include <stdlib.h>
#include <string.h>




//bash
int bash_explain(int,int);
int bash_start(char*,int);
int bash_stop(int);
int bash_init(char*,char*);
int bash_kill();
//c
int c_explain(int,int);
int c_start(char*,int);
int c_stop(int);
int c_init(char*,char*);
int c_kill();
//cpp
int cpp_explain(int,int);
int cpp_start(char*,int);
int cpp_stop(int);
int cpp_init(char*,char*);
int cpp_kill();
//class
int class_explain(int,int);
int class_start(char*,int);
int class_stop(int);
int class_init(char*,char*);
int class_kill();
//none(example)
int none_explain(int,int);
int none_start(char*,int);
int none_stop(int);
int none_init(char*,char*);
int none_kill();
//dts
int dts_explain(int,int);
int dts_start(char*,int);
int dts_stop(int);
int dts_init(char*,char*);
int dts_kill();
//go
int go_explain(int,int);
int go_start(char*,int);
int go_stop(int);
int go_init(char*,char*);
int go_kill();
//include
int include_explain(int,int);
int include_start(char*,int);
int include_stop(int);
int include_init(char*,char*);
int include_kill();
//makefile
int makefile_explain(int,int);
int makefile_start(char*,int);
int makefile_stop(int);
int makefile_init(char*,char*);
int makefile_kill();
//java
int java_explain(int,int);
int java_start(char*,int);
int java_stop(int);
int java_init(char*,char*);
int java_kill();
//js
int js_explain(int,int);
int js_start(char*,int);
int js_stop(int);
int js_init(char*,char*);
int js_kill();
//json
int json_explain(int,int);
int json_start(char*,int);
int json_stop(int);
int json_init(char*,char*);
int json_kill();
//perl
int perl_explain(int,int);
int perl_start(char*,int);
int perl_stop(int);
int perl_init(char*,char*);
int perl_kill();
//php
int php_explain(int,int);
int php_start(char*,int);
int php_stop(int);
int php_init(char*,char*);
int php_kill();
//python
int python_explain(int,int);
int python_start(char*,int);
int python_stop(int);
int python_init(char*,char*);
int python_kill();
//ruby
int ruby_explain(int,int);
int ruby_start(char*,int);
int ruby_stop(int);
int ruby_init(char*,char*);
int ruby_kill();
//struct
int struct_explain(int,int);
int struct_start(char*,int);
int struct_stop(int);
int struct_init(char*,char*);
int struct_kill();








//filled when in main()
struct worker
{
	//[0,f]
	char suffix[8];
	char worker[8];

	//[10,1f]
        int (*start)();
	char pad0[8-sizeof(char*)];
        int (*stop)(int);
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
static struct worker worker[20];








int main(int argc,char *argv[])  
{
	//--------------------------help---------------------------
	if(argc==1)
	{
		printf("code2seed(target=? seed=? suffix=? worker=?)\n{\n");
		printf("	code2seed.exe .\n");
		printf("	code2seed.exe 1.cpp\n");
		printf("	code2seed.exe target=. suffix=.dts\n");
		printf("	code2seed.exe target=/your/dir suffix=.h worker=dts\n");
		printf("	code2seed.exe target=/usr/src/linux seed=2.txt suffix=.c worker=purec\n");
		printf("}\n");
		return 0;
	}
	//------------------------------------------------------------



}
/*
	//-----------------------分析输入开始-------------------------
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//target=
		if(	(p[0]=='t') &&
			(p[1]=='a') &&
			(p[2]=='r') &&
			(p[3]=='g') &&
			(p[4]=='e') &&
			(p[5]=='t') &&
			(p[6]=='=') )
		{
			printf("code=%s\n",p+7);
			snprintf(targetname,256,"%s",p+7);
			continue;
		}

		//seed=
		else if((p[0]=='s') &&
			(p[1]=='e') &&
			(p[2]=='e') &&
			(p[3]=='d') &&
			(p[4]=='=') )
		{
			printf("seed=%s\n",p+5);
			snprintf(seedname,256,"%s",p+5);
			continue;
		}

		//suffix=
		else if((p[0]=='s') &&
			(p[1]=='u') &&
			(p[2]=='f') &&
			(p[3]=='f') &&
			(p[4]=='i') &&
			(p[5]=='x') &&
			(p[6]=='=') )
		{
			printf("suffix=%s\n",p+7);
			snprintf(suffix,16,"%s",p+7);
			length=strlen(suffix);
		}

		//worker=
		else if((p[0]=='w') &&
			(p[1]=='o') &&
			(p[2]=='r') &&
			(p[3]=='k') &&
			(p[4]=='e') &&
			(p[5]=='r') &&
			(p[6]=='=') )
		{
			printf("worker=%s\n",p+7);
			snprintf(worker,16,"%s",p+7);
			continue;
		}

		//treat this as suffix
		else if( (p[0]=='.') && (p[1]!=0) )
		{
			printf("suffix=%s\n",p);
			snprintf(suffix,16,"%s",p);
			length=strlen(suffix);
		}

		//treat this as targetname
		else
		{
			//.c	.cpp	.h
			printf("target=%s\n",p);
			snprintf(targetname,256,"%s",p);
		}
	}
	//----------------------分析输入结束---------------------




	//------------------------检查开始---------------------
	if(targetname[0]==0)
	{
		printf("target=.\n");
		snprintf(targetname,256,".");
	}
	if(seedname[0]==0)
	{
		printf("seed=code.seed\n");
		snprintf(seedname,256,"code.seed");
	}
	if(suffix[0]==0)
	{
		//试着从targetname名字里面拿到后缀名
		p=0;
		for(i=0;i<256;i++)
		{
			if(targetname==0)break;
			if(targetname[i]=='.')p=targetname+i;
		}

		if( (p==0) | (p[1]==0) )
		{
			snprintf(suffix,16,".c");
			length=2;
		}
		else
		{
			snprintf(suffix,16,"%s",p);
			length=strlen(suffix);
		}
		printf("suffix=%s\n",p);
	}
	if(worker[0]==0)
	{
		if(strcmp(suffix,".c")==0)
		{
			printf("worker=purec\n");
			snprintf(worker,16,"purec");	//c
		}
		else if(strcmp(suffix,".cc")==0)
		{
			printf("worker=cpp\n");
			snprintf(worker,16,"cpp");	//c++
		}
		else if(strcmp(suffix,".cpp")==0)
		{
			printf("worker=cpp\n");
			snprintf(worker,16,"cpp");	//c++
		}
		else if(strcmp(suffix,".dts")==0)
		{
			printf("worker=dts\n");
			snprintf(worker,16,"dts");	//device tree
		}
		else if(strcmp(suffix,".h")==0)
		{
			printf("worker=struct\n");
			snprintf(worker,16,"struct");	//structure
		}
		else
		{
			printf("worker=none\n");
			snprintf(worker,16,"none");	//byte offset & line number
		}
	}
	//------------------------检查结束------------------------




	//------------------------阅读开始-------------------------
	printf("seed generating.................\n");
	dest=open(seedname, O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IRWXU|S_IRWXG|S_IRWXO);

	fileordir( targetname );

	close(seedfd);
	printf("seed generated\n");
	//-----------------------阅读结束-------------------------
}
*/
