#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void iterator_init();
void string_init(int argc,char** argv);




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



	iterator_init(argc,argv);
	string_init(argc,argv);
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
