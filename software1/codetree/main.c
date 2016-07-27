#include "main.h"




//target
static int targetfd=-1;
static char targetname[256]={0};

//seed
static int seedfd=-1;
static char seedname[256]={0};
static unsigned long long seedcount=0;

//file suffix
static int length=0;
static char suffix[256]={0};

//file worker
static char worker[256]={0};
int (*explain_start)();
int (*explain_stop)(int);
int (*explain_ing)(int,int);

//
static struct stat statbuf;
static unsigned char datahome[0x2000];	//4k+4k








//
void explainfile(char* thisfile,unsigned long long size)
{
	int ret=0;
	int countbyte=0;
	int start=0;
	int end=0;

//_______________________open+start___________________________
	//open
	targetfd=open(thisfile , O_RDONLY|O_BINARY);
	if(targetfd<0){printf("open fail\n");exit(-1);}

	//start
	explain_start(thisfile,size);
//_________________________________________________________




//______________________explain start__________________________
	//<=4k
	if(size<=0x1000)
	{
		//printf("@[%x,%llx):\n",0,size);
		ret=read(targetfd,datahome,size);
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
			ret=read(targetfd,datahome,0x2000);
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
				ret=read(targetfd,datahome+0x1000,0x1000);
				if(ret<0)
				{
					printf("readfail3\n");
					exit(-1);
				}
			}

			//文件还剩最后一点没读
			else if( end > 0x1000 )
			{
				ret=read(targetfd,datahome+0x1000,end-0x1000);
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

		//这里传的第一个参数是相对buffer的偏移量
		start=explain_ing(start,0x1000);

		//next or not
		countbyte += 0x1000;
		if( countbyte > size )break;

	}//while(1)
//______________________explain end________________________




theend:
	explain_stop(countbyte+start);
	close(targetfd);
	return;
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
		printf("wrong targetname!!!!!!!!!!\n");
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
	targetname[0]=seedname[0]=suffix[0]=worker[0]=0;




	//***********************help*************************
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
	//****************************************************




	//********************分析输入开始*********************
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
	//*******************分析输入结束******************




	//********************检查开始********************
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
	//********************检查结束**********************




seedgenerating:
	printf("seed generating.................\n");
	if(strcmp(worker,"purec")==0)
	{
		c_init(seedname,datahome);

		explain_start= c_start;
		explain_stop = c_stop;
		explain_ing  = c_explain;
		fileordir( targetname );

		c_kill();
	}
	else if(strcmp(worker,"cpp")==0)
	{
		cpp_init(seedname,datahome);

		explain_start= cpp_start;
		explain_stop = cpp_stop;
		explain_ing  = cpp_explain;
		fileordir( targetname );

		cpp_kill();
	}
	else if(strcmp(worker,"none")==0)
	{
		none_init(seedname,datahome);

		explain_start= none_start;
		explain_stop = none_stop;
		explain_ing  = none_explain;
		fileordir( targetname );

		none_kill();
	}
	else if(strcmp(worker,"dts")==0)
	{
		dts_init(seedname,datahome);

		explain_start= dts_start;
		explain_stop = dts_stop;
		explain_ing  = dts_explain;
		fileordir( targetname );

		dts_kill();
	}
	else if(strcmp(worker,"include")==0)
	{
		include_init(seedname,datahome);

		explain_start= include_start;
		explain_stop = include_stop;
		explain_ing  = include_explain;
		fileordir( targetname );

		include_kill();
	}
	else if(strcmp(worker,"struct")==0)
	{
		struct_init(seedname,datahome);

		explain_start= struct_start;
		explain_stop = struct_stop;
		explain_ing  = struct_explain;
		fileordir( targetname );

		struct_kill();
	}
	printf("seed generated\n");
	//**************************************************
}
