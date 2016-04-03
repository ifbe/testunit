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




//code
static int codefd=-1;
static char codename[256]={0};

//hash
static int hashfd=-1;
static char hashname[256]={0};
static unsigned long long hashcount=0;

//seed
static int seedfd=-1;
static char seedname[256]={0};
static unsigned long long seedcount=0;

//file suffix
static int length=0;
static char suffix[256]={0};

//file worker
static char worker[256]={0};
void (*explain_start)();
void (*explain_stop)(int);
int (*explain_ing)(int,int);

//
static unsigned int lineoffset=0;
static unsigned int byteoffset=0;
static unsigned int badhash=0;
static unsigned int goodhash=0;

//
static struct stat statbuf;
static unsigned char datahome[0x2000];	//4k+4k
static unsigned char* buf;








//
void explainfile(char* thisfile,unsigned long long size)
{
	int ret=0;
	int countbyte=0;
	int start=0;
	int end=0;

//_______________________open+start___________________________
	//open
	codefd=open(thisfile , O_RDONLY|O_BINARY);
	if(codefd<0){printf("open fail\n");exit(-1);}

	//infomation
	ret=snprintf(datahome,256,"#name:	%s\n",thisfile);
	printf("%s",datahome);
	write(seedfd,datahome,ret);

	ret=snprintf(datahome,256,"#size:	%lld(0x%llx)\n",size,size);
	printf("%s",datahome);
	write(seedfd,datahome,ret);

	//start
	explain_start();
//_________________________________________________________




//______________________explain start__________________________
	//<=4k
	if(size<=0x1000)
	{
		//printf("@[%x,%llx):\n",0,size);
		ret=read(codefd,datahome,size);
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
		explain_ing(0,size);

		goto theend;
	}

	//>4k
	while(1)
	{
		//如果首次进来，那么读8k
		if(countbyte==0)
		{
			ret=read(codefd,datahome,0x2000);
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
				ret=read(codefd,datahome+0x1000,0x1000);
				if(ret<0)
				{
					printf("readfail3\n");
					exit(-1);
				}
			}

			//文件还剩最后一点没读
			else if( end > 0x1000 )
			{
				ret=read(codefd,datahome+0x1000,end-0x1000);
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
		start=explain_ing(start,0x1000);

		//next or not
		countbyte += 0x1000;
		if( countbyte > size )break;

	}//while(1)
//______________________explain end________________________




//______________________stop+close___________________________
theend:
	explain_stop(countbyte+start);
	close(codefd);
	return;
//_____________________________________________________
}
void fileordir(char* thisname)
{
	DIR*		thisfolder;
	struct dirent*	ent;

	int	i=0,j=0;
	char	childpath[256];

	//看看是否存在
	i=stat( thisname , &statbuf );
	if(i == -1)
	{
		printf("wrong codename!!!!!!!!!!\n");
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
	//数据特别大，用int会溢出
	unsigned long long i=0;
	unsigned long long ret=0;
	unsigned long long percent=0;
	unsigned long long length=0;

	//stat
	ret=stat( seedname , &statbuf );
	if(ret == -1)
	{
		printf("wrong seedname\n");
		exit(-1);
	}

	//malloc
	buf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(buf==NULL)
	{
		printf("malloc failed1\n");
		exit(-1);
	}

	//open
	hashfd=open(hashname,O_CREAT|O_RDWR|O_TRUNC|O_BINARY,S_IRWXU|S_IRWXG|S_IRWXO);
	seedfd=open(seedname , O_RDONLY|O_BINARY);
	if(seedfd<0)
	{
		printf("open failed\n");
		free(buf);
		exit(-1);
	}

	//read
	seedcount=read(seedfd,buf,statbuf.st_size);
	if(seedcount!=statbuf.st_size)
	{
		printf("read failed\n");
		close(seedfd);
		free(buf);
		exit(-1);
	}




	//start generating
	percent=1;
	for(i=0;i<seedcount;i++)
	{
		if( ( (i*100) / seedcount ) >= percent)
		{
			printf("%llx/%llx(%lld%%)..............\r",i,seedcount,percent);
			fflush(stdout);
			percent++;
		}

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
			write(hashfd,&lineoffset,4);
			write(hashfd,&byteoffset,4);
			write(hashfd,&badhash,4);
			write(hashfd,&goodhash,4);

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

	close(hashfd);
	printf("\n");

}//seed2hash
void sorthash()
{
	//数据特别大，用int会溢出
	unsigned long long ii=0;
	unsigned long long jj=0;
	unsigned long long min=0;
	unsigned long long max=0;
	unsigned long long ret=0;
	unsigned long long percent=0;
	unsigned int temp1;
	unsigned int temp2;
	unsigned int temp3;
	unsigned int temp4;

	//stat
	ret=stat( hashname , &statbuf );
	if(ret == -1)
	{
		printf("wrong seedname\n");
		return;
	}

	//malloc
	buf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(buf==NULL)
	{
		printf("malloc failed1\n");
		return;
	}

	//open
	hashfd=open(hashname , O_RDONLY|O_BINARY);
	if(hashfd<0)
	{
		printf("open failed\n");
		free(buf);
		return;
	}

	//read
	hashcount=read(hashfd,buf,statbuf.st_size);
	if(hashcount!=statbuf.st_size)
	{
		printf("read failed\n");
		close(hashfd);
		free(buf);
		return;
	}

	//close
	close(hashfd);




	//open
	hashfd=open(hashname,O_CREAT|O_RDWR|O_TRUNC|O_BINARY,S_IRWXU|S_IRWXG|S_IRWXO);




	//sort "goodhash"
	percent=1;
	for(ii=0;ii<hashcount;ii+=0x10)
	{
		if( ( (ii*100) / hashcount ) >= percent)
		{
			printf("%llx/%llx(%lld%%).............\r",ii,hashcount,percent);
			fflush(stdout);
			percent++;
		}

		//min address , min value
		ret=ii;
		temp1=*(unsigned int*)(buf+ii+0xc);

		//search smaller
		for(jj=ii+0x10;jj<hashcount;jj+=0x10)
		{
			temp2=*(unsigned int*)(buf+jj+0xc);
			if(temp2<temp1)
			{
				ret=jj;
				temp1=temp2;
			}
		}

		//swap(this,smallest)
		//printf("%x\n",temp1);
		temp1=*(unsigned int*)(buf+ii+0x0);
		temp2=*(unsigned int*)(buf+ii+0x4);
		temp3=*(unsigned int*)(buf+ii+0x8);
		temp4=*(unsigned int*)(buf+ii+0xc);
		*(unsigned int*)(buf+ii+0x0)=*(unsigned int*)(buf+ret+0x0);
		*(unsigned int*)(buf+ii+0x4)=*(unsigned int*)(buf+ret+0x4);
		*(unsigned int*)(buf+ii+0x8)=*(unsigned int*)(buf+ret+0x8);
		*(unsigned int*)(buf+ii+0xc)=*(unsigned int*)(buf+ret+0xc);
		*(unsigned int*)(buf+ret+0x0)=temp1;
		*(unsigned int*)(buf+ret+0x4)=temp2;
		*(unsigned int*)(buf+ret+0x8)=temp3;
		*(unsigned int*)(buf+ret+0xc)=temp4;
	}
	printf("\n");




	//sort "badhash"
	for(min=0;min<hashcount;min+=0x10)
	{
		temp1=*(unsigned int*)(buf+min+0xc);
		for(max=min;max<hashcount;max+=0x10)
		{
			temp2=*(unsigned int*)(buf+max+0xc);
			if(temp2!=temp1)break;
		}
		if(max==min+0x10)continue;

		//sort
		for(ii=min;ii<max;ii+=0x10)
		{
			ret=ii;
			temp1=*(unsigned int*)(buf+ii+0x8);
			for(jj=ii+0x10;jj<max;jj+=0x10)
			{
				temp2=*(unsigned int*)(buf+jj+0x8);
				if(temp2<temp1)
				{
					ret=jj;
					temp1=temp2;
				}
			}

			//swap(this,smallest)
			//printf("%x\n",temp1);
			temp1=*(unsigned int*)(buf+ii+0x0);
			temp2=*(unsigned int*)(buf+ii+0x4);
			temp3=*(unsigned int*)(buf+ii+0x8);
			temp4=*(unsigned int*)(buf+ii+0xc);
			*(unsigned int*)(buf+ii+0x0)=*(unsigned int*)(buf+ret+0x0);
			*(unsigned int*)(buf+ii+0x4)=*(unsigned int*)(buf+ret+0x4);
			*(unsigned int*)(buf+ii+0x8)=*(unsigned int*)(buf+ret+0x8);
			*(unsigned int*)(buf+ii+0xc)=*(unsigned int*)(buf+ret+0xc);
			*(unsigned int*)(buf+ret+0x0)=temp1;
			*(unsigned int*)(buf+ret+0x4)=temp2;
			*(unsigned int*)(buf+ret+0x8)=temp3;
			*(unsigned int*)(buf+ret+0xc)=temp4;
		}
	}




	//write,close,free
	write(hashfd,buf,hashcount);
	close(hashfd);
	free(buf);
}









int main(int argc,char *argv[])  
{
	int i;
	char* p;
	codename[0]=seedname[0]=suffix[0]=worker[0]=0;




	//***********************help*************************
	if(argc==1)
	{
		printf("code2seed(code=? seed=? suffix=? worker=?)\n{\n");
		printf("	code2seed.exe 1.c\n");
		printf("	code2seed.exe .cpp\n");
		printf("	code2seed.exe code=/usr/src/linux seed=2.txt suffix=.c worker=purec\n");
		printf("}\n");
		return 0;
	}
	//****************************************************




	//********************分析输入开始*********************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//code=
		if(	(p[0]=='c') &&
			(p[1]=='o') &&
			(p[2]=='d') &&
			(p[3]=='e') &&
			(p[4]=='=') )
		{
			printf("code=%s\n",p+5);
			snprintf(codename,256,"%s",p+5);
			continue;
		}

		//hash=
		else if((p[0]=='h') &&
			(p[1]=='a') &&
			(p[2]=='s') &&
			(p[3]=='h') &&
			(p[4]=='=') )
		{
			printf("hash=%s\n",p+5);
			snprintf(hashname,256,"%s",p+5);
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
		else if(p[0]=='.')
		{
			printf("suffix=%s\n",p);
			snprintf(suffix,16,"%s",p);
			length=strlen(suffix);
		}

		//treat this as codename
		else
		{
			//.c	.cpp	.h
			printf("code=%s\n",p);
			snprintf(codename,256,"%s",p);
		}
	}
	//*******************分析输入结束******************




	//********************检查开始********************
	if(codename[0]==0)
	{
		printf("code=.\n");
		snprintf(codename,256,".");
	}
	if(hashname[0]==0)
	{
		printf("hash=code.hash\n");
		snprintf(hashname,256,"code.hash");
	}
	if(seedname[0]==0)
	{
		printf("seed=code.seed\n");
		snprintf(seedname,256,"code.seed");
	}
	if(suffix[0]==0)
	{
		//种子已经有了，只想要哈希就跳过这些
		if(strcmp(worker,"hash")==0)
		{
			goto hashgenerating;
		}

		//试着从codename名字里面拿到后缀名
		p=0;
		for(i=0;i<256;i++)
		{
			if(codename==0)break;
			if(codename[i]=='.')p=codename+i;
		}
		if( (p==0) | (p[1]==0) )
		{
			printf("invalid suffix!!!!!!!!!!!\n");
			return 0;
		}

		//
		printf("suffix=%s\n",p);
		snprintf(suffix,16,"%s",p);
		length=strlen(suffix);
	}
	if(worker[0]==0)
	{
		//set up default worker
		/*
		if(.c)purec
		if(.cc)cpp
		if(.cpp)cpp
		if(.h)struct
		if(.hh)class
		if(.java)java
		*/
		if(strcmp(suffix,".c")==0)
		{
			printf("worker=purec\n");
			snprintf(worker,16,"purec");
		}
		else if(strcmp(suffix,".cc")==0)
		{
			printf("worker=cpp\n");
			snprintf(worker,16,"cpp");
		}
		else if(strcmp(suffix,".cpp")==0)
		{
			printf("worker=cpp\n");
			snprintf(worker,16,"cpp");
		}
		else if(strcmp(suffix,".h")==0)
		{
			printf("worker=struct\n");
			snprintf(worker,16,"struct");
		}
		else
		{
			printf("invalid worker!!!!!!!!!!\n");
			return 0;
		}
	}
	//********************检查结束**********************




seedgenerating:
	//*********************1:code2seed***********************
	printf("seed generating.................\n");
	seedfd=open(
		seedname,
		O_CREAT|O_RDWR|O_TRUNC|O_BINARY,
		S_IRWXU|S_IRWXG|S_IRWXO
	);

	if(strcmp(worker,"purec")==0)
	{
		//open
		initpurec(seedfd,datahome);
		explain_start=startpurec;
		explain_stop=stoppurec;
		explain_ing=explainpurec;
	}
	else if(strcmp(worker,"cpp")==0)
	{
		initcpp(seedfd,datahome);
		explain_start=startcpp;
		explain_stop=stopcpp;
		explain_ing=explaincpp;
	}
	else if(strcmp(worker,"struct")==0)
	{
		initstruct(seedfd,datahome);
		explain_start=startstruct;
		explain_stop=stopstruct;
		explain_ing=explainstruct;
	}

	//do it
	fileordir( codename );

	//close
	close(seedfd);
	printf("seed generated\n");
	//**************************************************




hashgenerating:
	//**********************2.seed2hash*********************
	printf("hash generating................\n");
	generatehash();
	printf("hash generated\n");
	//****************************************************




hashsorting:
	//********************.hash sorting*******************
	printf("hash sorting..................\n");
	sorthash();
	printf("hash sorted\n");
	//****************************************************
}
