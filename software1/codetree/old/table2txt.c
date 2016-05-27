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




//
static int dest=-1;
static int src=-1;

//
static char* buf=0;
static int count=0;

//输入文件名
static char infile[256]={0};

//输出文件名
static char outfile[256]={0};




//(char* , int) -> (int* , int)
char outbuf[256];
void table2txt()
{
	int i;
	int ret;
	unsigned int* p;
	printf("count=%d\n",count);

	for(i=0;i<count;i+=0x20)
	{
		p=(unsigned int*)(buf+i);
		ret=snprintf(outbuf,256,
			"(%.8x,%.8x),(%.8x,%.8x),(%.8x,%.8x)@(%d,%d)\n",
			p[0],p[1],
			p[2],p[3],
			p[4],p[5],
			p[6],p[7]
		);
		write(dest,outbuf,ret);
	}//for
}//seed2table




int main(int argc,char *argv[])  
{
	//
	char* p;
	int i;
	int ret;
	struct stat	statbuf;
	infile[0]=outfile[0]=0;




	//************************help*************************
	if(argc==1)
	{
		printf("usage:\n");
		printf("table2txt infile=code.table outfile=code.txt\n");
		return 0;
	}
	//******************************************************




	//*****************************************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//infile=
		if(	(p[0]=='i') &&
			(p[1]=='n') &&
			(p[2]=='f') &&
			(p[3]=='i') &&
			(p[4]=='l') &&
			(p[5]=='e') &&
			(p[6]=='=') )
		{
			printf("infile=%s\n",p+7);
			snprintf(infile,16,"%s",p+7);
		}

		//outfile=
		if(	(p[0]=='o') &&
			(p[1]=='u') &&
			(p[2]=='t') &&
			(p[3]=='f') &&
			(p[4]=='i') &&
			(p[5]=='l') &&
			(p[6]=='e') &&
			(p[7]=='=') )
		{
			printf("outfile=%s\n",p+8);
			snprintf(outfile,16,"%s",p+8);
		}
	}
	//*************************************




	//****************************************
	if(infile[0]==0)
	{
		printf("infile=code.table\n");
		snprintf(infile,16,"code.table");
	}
	if(outfile[0]==0)
	{
		printf("outfile=code.txt\n");
		snprintf(outfile,16,"code.txt");
	}
	//******************************************




	//check
	ret=stat( infile , &statbuf );
	if(ret == -1)
	{
		printf("wrong infile\n");
		goto statfailed;
	}

	//open
	dest=open(outfile,O_CREAT|O_RDWR|O_TRUNC|O_BINARY,S_IRWXU|S_IRWXG|S_IRWXO);
	src=open(infile , O_RDONLY|O_BINARY);
	if(src<0)
	{
		printf("open failed\n");
		goto openfailed;
	}

	//malloc
	buf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(buf==NULL)
	{
		printf("malloc failed1\n");
		goto mallocfailed;
	}

	//read
	count=read(src,buf,statbuf.st_size);
	if(count!=statbuf.st_size)
	{
		printf("read failed\n");
		goto readfailed;
	}

	//process
	table2txt();

readfailed:
	free(buf);
mallocfailed:
	close(src);
	close(dest);
openfailed:
	return;
statfailed:
	return;
}
