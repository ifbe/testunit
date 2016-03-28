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

//¿¿¿¿¿
static char infile[256]={0};

//¿¿¿¿¿
static char outfile[256]={0};




//(char* , int) -> (int* , int)
void seed2table()
{
	unsigned int ancestoroffset=0;	//¿¿¿¿¿¿¿¿
	unsigned int ancestorlength=0;
	unsigned int fatheroffset=0;	//¿¿¿¿¿¿¿¿
	unsigned int fatherlength=0;
	unsigned int lineoffset=0;
	unsigned int linelength=0;
	unsigned int byteoffset=0;
	unsigned int bytelength=0;
	int i=0;
	int signal=0;

	for(i=0;i<count;i++)
	{
		//linux¿¿¿
		if(buf[i]==0xa)
		{
			lineoffset++;
			if(signal==0)continue;

			//¿¿¿¿¿
			write(dest,&ancestoroffset,4);
			write(dest,&ancestorlength,4);
			write(dest,&fatheroffset,4);
			write(dest,&fatherlength,4);
			write(dest,&lineoffset,4);
			write(dest,&linelength,4);
			write(dest,&byteoffset,4);
			write(dest,&bytelength,4);

			signal=0;
			continue;
		}
		else if(buf[i]==0xd)
		{
			lineoffset++;
			if(buf[i+1]==0xa)i++;
			if(signal=0)continue;

			//¿¿¿¿¿
			write(dest,&ancestoroffset,4);
			write(dest,&ancestorlength,4);
			write(dest,&fatheroffset,4);
			write(dest,&fatherlength,4);
			write(dest,&lineoffset,4);
			write(dest,&linelength,4);
			write(dest,&byteoffset,4);
			write(dest,&bytelength,4);

			signal=0;
			continue;
		}

		//¿¿¿¿¿¿¿¿¿
		else if(buf[i]=='#')
		{
			//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
			if(	(buf[i+1]=='n')&&
				(buf[i+2]=='a')&&
				(buf[i+3]=='m')&&
				(buf[i+4]=='e')&&
				(buf[i+5]==':')&&
				(buf[i+6]==0x9) )
			{
				i+=7;
				ancestoroffset=i;
				while(1)
				{
					if(buf[i+1]==0)break;
					else if(buf[i+1]==0xa)break;
					else if(buf[i+1]==0xd)break;

					i++;
				}
				ancestorlength=i-ancestoroffset+1;
				continue;
			}

			//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
			else
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
		}

		//¿¿¿¿¿
		else
		{
			if(buf[i]==0x9)
			{
				i++;
				fatheroffset=i;
				while(1)
				{
					if(buf[i+1]==0)break;
					else if(buf[i+1]==0x9)break;
					else if(buf[i+1]==0x20)break;
					else if(buf[i+1]==0xa)break;
					else if(buf[i+1]==0xd)break;

					i++;
				}
				fatherlength=i-fatheroffset+1;

				signal=1;
			}
			else
			{
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
				bytelength=i-byteoffset+1;

				signal=1;
			}

			//¿¿¿¿¿¿¿¿¿¿¿
			while(1)
			{
				if(buf[i+1]==0)break;
				else if(buf[i+1]==0xa)break;
				else if(buf[i+1]==0xd)break;

				i++;
			}
		}//else
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
		printf("seed2table infile=code.seed outfile=code.table\n");
		return 0;
	}
	//******************************************************




	//********************¿¿¿¿¿¿*********************
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
	//*******************¿¿¿¿¿¿******************




	//********************¿¿¿¿********************
	if(infile[0]==0)
	{
		printf("invalid infile,using code.seed\n");
		snprintf(infile,16,"code.seed");
	}
	if(outfile[0]==0)
	{
		printf("invalid outfile,using code.table\n");
		snprintf(outfile,16,"code.table");
	}
	//********************¿¿¿¿**********************




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
	buf[count]=0xa;
	buf[count+1]=0;
	seed2table();

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
