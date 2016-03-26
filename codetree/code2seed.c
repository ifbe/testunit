#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 
//c
int explainpurec(int,int);
void startpurec();
void stoppurec(int);
void initpurec(int,char*);
void killpurec();
//struct
int explainstruct(int,int);
void startstruct();
void stopstruct(int);
void initstruct(int,char*);
void killstruct();
//cpp
int explaincpp(int,int);
void startcpp();
void stopcpp(int);
void initcpp(int,char*);
void killcpp();
//class
int explainclass(int,int);
void startclass();
void stopclass(int);
void initclass(int,char*);
void killclass();
//java
int explainjava(int,int);
void startjava();
void stopjava(int);
void initjava(int,char*);
void killjava();




//
static unsigned char datahome[0x2000];	//4k+4k
static int dest=-1;
static int src=-1;

//
static char infile[256]={0};
	//input dir
static char outfile[256]={0};
	//output name
static char processor[256]={0};
	//file processor
static char suffix[256]={0};
static int length=0;
	//file suffix




//
void explainfile(char* thisfile,unsigned long long size)
{
	int ret=0;
	int countbyte=0;
	int start=0;
	int end=0;

//_______________________open+start___________________________
	//open
	src=open(thisfile , O_RDONLY|O_BINARY);
	if(src<0){printf("open fail\n");exit(-1);}

	//infomation
	ret=snprintf(datahome,256,"#name:	%s\n",thisfile);
	printf("%s",datahome);
	write(dest,datahome,ret);

	ret=snprintf(datahome,256,"#size:	%lld(0x%llx)\n",size,size);
	printf("%s",datahome);
	write(dest,datahome,ret);

	//start
	startpurec();
//_________________________________________________________




//______________________explain start__________________________
	//<=4k
	if(size<=0x1000)
	{
		//printf("@[%x,%llx):\n",0,size);
		ret=read(src,datahome,size);
		if(ret<0)
		{
			printf("readfail1\n");
			exit(-1);
		}

		//size=0x0 -> datahome[0x0]=0
		//size=0x1 -> datahome[0x1]=0
		//size=0xfff -> datahome[0xfff]=0
		//size=0x1000 -> datahome[0x1000]=0
		datahome[size]=0;
		explainpurec(0,size);

		goto theend;
	}

	//>4k
	while(1)
	{
		//如果首次进来，那么读8k
		if(countbyte==0)
		{
			ret=read(src,datahome,0x2000);
			if(ret<0)
			{
				printf("readfail2\n");
				exit(-1);
			}

			//补0
			if(ret<0x2000)datahome[ret]=0;
		}

		//如果不是首次，先挪，再看要不要读入4k
		else
		{
			//move
			for(ret=0;ret<0x1000;ret++)
			{
				datahome[ ret ] = datahome[ ret+0x1000 ];
			}
			end=size-countbyte;

			//文件还剩很多没读
			if( end > 0x2000 )
			{
				ret=read(src,datahome+0x1000,0x1000);
				if(ret<0)
				{
					printf("readfail3\n");
					exit(-1);
				}
			}

			//文件还剩最后一点没读
			else if( end > 0x1000 )
			{
				ret=read(src,datahome+0x1000,end-0x1000);
				if(ret<0)
				{
					printf("readfail4\n");
					exit(-1);
				}

				//补0
				if(ret<0x1000)datahome[0x1000+ret]=0;
			}

			//文件上一波就读完了，内存里还残留一点
			else if( end > 0 )
			{
				//补个0
				datahome[end]=0;
			}
		}

		//do it
		start=explainpurec(start,0x1000);

		//next or not
		countbyte += 0x1000;
		if( countbyte > size )break;

	}//while(1)
//______________________explain end________________________




//______________________stop+close___________________________
theend:
	stoppurec(countbyte+start);
	close(src);
        return;
//_____________________________________________________
}
void fileordir(char* thisname)
{
	DIR*		thisfolder;
	struct dirent*	ent;
	struct stat	statbuf;

	int	i=0,j=0;
	char	childpath[256];
	printf("i am here\n");

	//看看是否存在
	i=stat( thisname , &statbuf );
	if(i == -1)
	{
		printf("wrong file\n");
		return;
	}

	//如果是文件夹，就进去
	if(statbuf.st_mode & S_IFDIR)
	{
		thisfolder=opendir(thisname);
		memset(childpath,0,sizeof(childpath));
		while(1)
		{
			ent=readdir(thisfolder);
			if(ent==NULL)break;
			if(ent->d_name[0]=='.')continue;

			snprintf(childpath,256,"%s/%s",thisname,ent->d_name);
			fileordir(childpath);
		}//while1
		closedir(thisfolder);
	}

	//再看看是不是普通文件
	else
	{
		//没有后缀的不对
		i=j=0;
		while(1)
		{
			if(thisname[i]==0)break;
			if(thisname[i]=='.')j=i;
			i++;
		}
		if(j==0)return;

		//长度不够不对
		if(i-j<length)return;

		//长度超过也不对
		if( thisname[ j+length ] > 0x20 )return;

		//名字不一样不对
		if( strcmp( thisname+j , suffix ) != 0 )return;

		//文件空的也不对
		i=statbuf.st_size;
		if(i<=0)return;

		//是源代码就进去翻译
		explainfile(thisname,i);
		return;
	}
}//fileordir




int main(int argc,char *argv[])  
{
	int i;
	char* p;
	infile[0]=outfile[0]=processor[0]=suffix[0]=0;




	//***********************help*************************
	if(argc==1)
	{
		printf("usage:\n");
		printf("code2seed .c\n");
		printf("code2seed infile=filename.cc outfile=what.seed\n");
		printf("code2seed infile=/tmp/what.java outfile=what.seed\n");
		return 0;
	}
	//****************************************************




	//********************分析输入开始*********************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//.c	.cpp	.h
		if(p[0]=='.')
		{
			printf("suffix=%s\n",p);
			snprintf(suffix,16,"%s",p);
			length=strlen(suffix);
		}

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
	//*******************分析输入结束******************




	//********************检查开始********************
	if(suffix[0]==0)
	{
		if(infile[0]!=0)
		{
			//search infile for suffix
			p=0;
			for(i=0;i<256;i++)
			{
				if(infile==0)break;
				if(infile[i]=='.')p=infile+i;
			}
			if(p==0)
			{
				printf("invalid suffix1\n");
				return 0;
			}

			//
			printf("suffix=%s\n",p);
			snprintf(suffix,16,"%s",p);
			length=strlen(suffix);
		}
		else
		{
			printf("invalid suffix2\n");
			return 0;
		}
	}
	if(processor[0]==0)
	{
		//set up default processor
		/*
		if(.c)purec
		if(.cc)cpp
		if(.cpp)cpp
		if(.h)struct
		if(.hh)class
		if(.java)java
		*/
	}
	if(infile[0]==0)
	{
		printf("invalid infile,using .\n");
		snprintf(infile,16,".");
	}
	if(outfile[0]==0)
	{
		printf("invalid outfile,using code.seed\n");
		snprintf(outfile,16,"code.seed");
	}
	//********************检查结束**********************




	//**********************before**********************
	dest=open(outfile,O_CREAT|O_RDWR|O_TRUNC|O_BINARY,S_IRWXU|S_IRWXG|S_IRWXO);
	initpurec(dest,datahome);
	//***************************************************




	//********************processing***********************
	fileordir( infile );
	//***************************************************




	//**********************after************************
	close(dest);
	killpurec();
	//close(dest_cpp);
	//killcpp();
	//close(dest_struct);
	//killstruct();
	//close(dest_class);
	//killclass();
	//**************************************************
}
