#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 




//
static unsigned char datahome[0x2000];	//4k+4k
static int dest=-1;
static int src=-1;

//
static char in[256]={0};
	//输入文件名
static char out[256]={0};
	//输出文件名
static char type[256]={0};
static int typelen=0;
	//文件种类
static char worker[256]={0};
	//处理员




//
void explainfile(char* thisfile,unsigned long long size)
{
	int ret=0;
	int countbyte=0;
	int start=0;
	int end=0;

//_______________________open+start___________________________
	//open
	src=open(thisfile , O_RDONLY);
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
		if(i-j<typelen)return;

		//长度超过也不对
		if( thisname[ j+typelen ] > 0x20 )return;

		//名字不一样不对
		if( strcmp( thisname+j , type ) != 0 )return;

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
	in[0]=out[0]=type[0]=worker[0]=0;




	//***********************help*************************
	if(argc==1)
	{
		printf("usage:\n");
		printf("code2seed if=.c of=name.seed\n");
		printf("code2seed if=/mnt/code/linux/*.c of=name.seed\n");
		return 0;
	}
	//****************************************************




	//********************分析输入开始*********************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//type=
		if(	(p[0]=='t') &&
			(p[1]=='y') &&
			(p[2]=='p') &&
			(p[3]=='e') &&
			(p[4]=='=') )
		{
			printf("type=%s\n",p+5);
			snprintf(type,16,"%s",p+5);
			typelen=strlen(type);
		}

		//worker=
		if(	(p[0]=='w') &&
			(p[1]=='o') &&
			(p[2]=='r') &&
			(p[3]=='k') &&
			(p[4]=='e') &&
			(p[5]=='r') &&
			(p[6]=='=') )
		{
			printf("worker=%s\n",p+7);
			snprintf(worker,16,"%s",p+7);
		}

		//in=
		if(	(p[0]=='i') &&
			(p[1]=='n') &&
			(p[2]=='=') )
		{
			printf("in=%s\n",p+3);
			snprintf(in,16,"%s",p+3);
		}

		//out=
		if(	(p[0]=='o') &&
			(p[1]=='u') &&
			(p[2]=='t') &&
			(p[3]=='=') )
		{
			printf("out=%s\n",p+4);
			snprintf(out,16,"%s",p+4);
		}
	}
	//*******************分析输入结束******************




	//********************检查开始********************
	if(type[0]==0)
	{
		printf("invalid type\n");
		return 0;
	}
	if(worker[0]==0)
	{
		printf("invalid worker\n");
		return 0;
	}
	if(in[0]==0)
	{
		printf("invalid in,using .\n");
		snprintf(in,16,".");
	}
	if(out[0]==0)
	{
		printf("invalid out,using code.seed\n");
		snprintf(out,16,"code.seed");
	}
	//********************检查结束**********************




	//**********************before**********************
	dest=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	initpurec(dest,datahome);
	//dest_cpp=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	//initcpp(dest_cpp,datahome);
	//dest_struct=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	//initstruct(dest_struct,datahome);
	//dest_class=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	//initclass(dest_class,datahome);
	//***************************************************




	//********************processing***********************
	fileordir( in );
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
