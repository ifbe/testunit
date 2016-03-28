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
static struct stat statbuf;

//输入文件名
static char infile[256]={0};

//输出文件名
static char outfile[256]={0};

//
static char* buf=0;
static int count=0;

//
static unsigned int lineoffset=0;
static unsigned int byteoffset=0;
static unsigned int badhash=0;
static unsigned int goodhash=0;




void whathash(int start , int size)
{
	int i;
	badhash=0;
	goodhash=5381;
	for(i=start;i<start+size;i++)
	{
		badhash=badhash+buf[i];
		goodhash=(goodhash<<5)+goodhash+buf[i];
	}
	badhash=(badhash&0xffff)+(size<<16);
}
//(char* , int) -> (int* , int)
void generatehash()
{
	int i=0;
	int ret;
	unsigned int length=0;

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




	//start generating
	for(i=0;i<count;i++)
	{
		//linux
		if( (buf[i]==0xa) | (buf[i]==0xd) )
		{
			//deal with mac and windows problem
			if(buf[i]==0xd)
			{
				if(buf[i+1]==0xa)i++;
			}

			//line number
			lineoffset++;
			if(ret==0)continue;

			//hash
			whathash(byteoffset,length);

			//write
			write(dest,&lineoffset,4);
			write(dest,&byteoffset,4);
			write(dest,&badhash,4);
			write(dest,&goodhash,4);

			//next
			ret=0;
			continue;
		}

		//不要的全吃掉
		else if((buf[i]=='#') |
			(buf[i]=='{') |
			(buf[i]=='}') |
			(buf[i]==0x9) )
		{
			while(1)
			{
				if(buf[i+1]==0)break;
				else if(buf[i+1]==0xa)break;
				else if(buf[i+1]==0xd)break;

				i++;
			}
			continue;
		}

		//if(buf[i]==0x9)i++;

		byteoffset=i;
		while(1)
		{
			if(buf[i+1]==0)break;
			else if(buf[i+1]==0x9)break;
			else if(buf[i+1]==0x20)break;
			else if(buf[i+1]==0xa)break;
			else if(buf[i+1]==0xd)break;
			i++;
		}
		length=i-byteoffset+1;

		//
		while(1)
		{
			if(buf[i+1]==0)break;
			else if(buf[i+1]==0xa)break;
			else if(buf[i+1]==0xd)break;

			i++;
		}

		//
		ret=1;
	}//for




readfailed:
	free(buf);
mallocfailed:
	close(src);
	close(dest);
openfailed:
	return;
statfailed:
	return;
}//seed2hash




void checkhash()
{
	int ret=0;
	int i=0;
	int j=0;
	unsigned int temp1;
	unsigned int temp2;
	unsigned int temp3;
	unsigned int temp4;

	//check
	ret=stat( outfile , &statbuf );
	if(ret == -1)
	{
		printf("wrong infile\n");
		goto statfailed;
	}

	//open
	src=open(outfile , O_RDONLY|O_BINARY);
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




	//start checking
	ret=0;
	for(i=0;i<count;i+=0x10)
	{
		temp1=*(unsigned int*)(buf+i+0xc);
		//printf("%.8llx\n",temp);
		for(j=0;j<count;j+=0x10)
		{
			//goodhash compare
			temp2=*(unsigned int*)(buf+j+0xc);
			if(temp1 != temp2)continue;

			//self compare
			if(i==j)continue;

			//badhash compare
			temp3=*(unsigned int*)(buf+i+8);
			temp4=*(unsigned int*)(buf+j+8);
			if(temp3 != temp4)continue;

			if(ret==0)
			{
				printf("%d\n" , *(unsigned int*)(buf+i) );
				ret=1;
			}
			printf(	"	%d\n" , *(unsigned int*)(buf+j) );
		}//for j

		ret=0;
	}//for i




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




int main(int argc,char *argv[])  
{
	//
	char* p;
	int i;
	infile[0]=outfile[0]=0;




	//************************help*************************
	if(argc==1)
	{
		printf("usage:\n");
		printf("seed2hash infile=code.seed outfile=code.table\n");
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
		printf("infile=code.seed\n");
		snprintf(infile,16,"code.seed");
	}
	if(outfile[0]==0)
	{
		printf("outfile=code.hash\n");
		snprintf(outfile,16,"code.hash");
	}
	//******************************************




	//generate
	printf("generating........\n");
	generatehash();
	printf("generated\n");

	//check
	printf("checking........\n");
	checkhash();
	printf("checked\n");
}
