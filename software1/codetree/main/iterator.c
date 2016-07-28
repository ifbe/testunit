#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef O_BINARY
        //mingw64 compatiable
        #define O_BINARY 0x0
#endif




//
static struct stat statbuf;
static unsigned char datahome[0x200000];

//
static int targetfd=-1;
static int seedfd=-1;




//
void explainfile(char* filename,unsigned long long size)
{
	int ret=0;
	int countbyte=0;
	int start=0;
	int end=0;

//_______________________open+start___________________________
	//open
	targetfd=open(filename , O_RDONLY|O_BINARY);
	if(targetfd<0){printf("open fail\n");exit(-1);}

	//start
	//explain_start(filename,size);
//_________________________________________________________




//______________________explain start__________________________
	//<=4k
	if(size<=0x100000)
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
		//explain_ing(0,size);

		goto theend;
	}

	//>4k
	while(1)
	{
		//如果首次进来，那么读8k
		if(countbyte==0)
		{
			ret=read(targetfd,datahome,0x200000);
			if(ret<0)
			{
				printf("readfail2\n");
				exit(-1);
			}

			//补0
			if(ret<0x200000)datahome[ret]=0;
		}

		//如果不是首次，先挪，再看要不要读入4k
		else
		{
			//move
			for(ret=0;ret<0x100000;ret++)
			{
				datahome[ ret ] = datahome[ ret+0x100000 ];
			}
			end=size-countbyte;

			//文件还剩很多没读
			if( end > 0x200000 )
			{
				ret=read(targetfd,datahome+0x100000,0x100000);
				if(ret<0)
				{
					printf("readfail3\n");
					exit(-1);
				}
			}

			//文件还剩最后一点没读
			else if( end > 0x100000 )
			{
				ret=read(targetfd,datahome+0x100000,end-0x100000);
				if(ret<0)
				{
					printf("readfail4\n");
					exit(-1);
				}

				//补0
				if(ret<0x100000)datahome[0x100000+ret]=0;
			}

			//文件上一波就读完了，内存里还残留一点
			else if( end > 0 )
			{
				//补个0
				datahome[end]=0;
			}
		}

		//这里传的第一个参数是相对buffer的偏移量
		//start=explain_ing(start,0x100000);

		//next or not
		countbyte += 0x100000;
		if( countbyte > size )break;

	}//while(1)
//______________________explain end________________________




theend:
	//explain_stop(countbyte+start);
	close(targetfd);
	return;
}
void fileordir(char* filename)
{
	DIR*		thisfolder;
	struct dirent*	ent;

	int	i=0,j=0;
	char	childpath[256];

	//看看是否存在
	i=stat( filename , &statbuf );
	if(i == -1)
	{
		printf("wrong targetname!!!!!!!!!!\n");
		return;
	}

	//如果是文件夹，就进去
	if(statbuf.st_mode & S_IFDIR)
	{
		thisfolder=opendir(filename);
		memset(childpath,0,sizeof(childpath));
		while(1)
		{
			ent=readdir(thisfolder);
			if(ent==NULL)break;
			if(ent->d_name[0]=='.')continue;

			snprintf(childpath,256,"%s/%s",filename,ent->d_name);
			fileordir(childpath);
		}//while1
		closedir(thisfolder);
	}

	//再看看是不是普通文件
	else
	{
		//文件空的不管
		if(statbuf.st_size<=0)return;

		//后缀不对的不管
		if(match( eatdir(filename) , ".c" ) == 0)return;

		//是源代码就进去翻译
		explainfile(filename,i);
		return;
	}
}//fileordir
