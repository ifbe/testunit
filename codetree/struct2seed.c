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
static unsigned char strbuf[256];
static int dest=-1;
static int src=-1;

//
static int countbyte=0;		//统计字节数
static int countline=0;		//统计行数

//
static int instruct=0;    //在函数内
static int inmarco=0;   //在宏内
static int innote=0;    //在注释内
static int instr=0;     //在字符串内




int explainheader(int start,int end)
{
	int i=0;
	unsigned ch=0;
	printf(
		"@%x:%d -> %d,%d,%d,%d\n",
		countbyte+start,
		countline+1,
		instruct,
		inmarco,
		innote,
		instr
	);

	//
	for(i=start;i<0x1800;i++)
	{
		//拿一个
		ch=datahome[i];
		//printf("%c",ch);

		//软退
		if( (i>end) )
		{
			if(ch==' ')break;
			else if(ch==0x9)break;
			else if(ch==0xa)break;
			else if(ch==0xd)break;
		}

		//强退(代码里绝不会有真正的0，都是ascii的0x30)
		if(ch==0)
		{
                        //printf("@%x\n",i);
                        break;
                }

		//在这里记录行数？
		else if( (ch==0xa)|(ch==0xd) )
		{
			//
			countline++;

			//单行注释，换行清零
			if(innote==1)innote=0;
		}

		//.....................
		else if(ch=='\\')
		{
			//吃掉一个
			i++;
			if( (datahome[i]==0xa)|(datahome[i]==0xd) )
			{
				countline++;
			}
			continue;
		}
                else if(ch=='\"')
                {
                        if(innote>0)continue;
                        if( instr==0 )
                        {
                                instr=1;
                        }
                        else if(instr==1)
                        {
                                instr=0;
                        }
                }
                else if(ch=='\'')
                {
                        if(innote>0|instr>0)continue;

                        while(1)
                        {
                                i++;
                                if(datahome[i]=='\'')break;
                                if(datahome[i]=='\\')i++;
                        }
                }
                else if(datahome[i]=='/')
                {
                        //在这三种情况下什么都不能干
                        if(innote>0|instr>0)continue;

                        //单行注释很好解决
                        if(datahome[i+1]=='/')  //    //
                        {
                                innote=1;
                        }

                        //多行注释
                        else if(datahome[i+1]=='*')     //    /*
                        {
                                innote=9;
                        }
                }
                else if(datahome[i]=='*')
                {
                        if(instr>0)continue;

                        if(datahome[i+1]=='/')
                        {
                                if(innote==9)
                                {
                                        innote=0;
                                }
                        }
                }
		else if(ch=='s')
		{
			if(innote>0|instr>0)continue;

			if( (datahome[i+1]=='t') &&
			    (datahome[i+2]=='r') &&
			    (datahome[i+3]=='u') &&
			    (datahome[i+4]=='c') &&
			    (datahome[i+5]=='t') )
			{
				if( (datahome[i+6]==' ') | 
				    (datahome[i+6]=='	') )
				printf("struct@%d\n",countline+1);
			}
		}
	}//for

	return i-end;
}
void explainfile(char* thisfile,unsigned long long size)
{
	int start=0;
	int end=0;
	int ret=0;

	//init
	countbyte=countline=0;
	instruct=inmarco=innote=instr=0;

	//open
	src=open(thisfile , O_RDONLY);
	if(src<0){printf("open fail\n");exit(-1);}

	//infomation
	ret=snprintf(strbuf,256,"#name:	%s\n",thisfile);
	printf("%s",strbuf);
	write(dest,strbuf,ret);

	ret=snprintf(strbuf,256,"#size:	%lld(0x%llx)\n",size,size);
	printf("%s",strbuf);
	write(dest,strbuf,ret);

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
		explainheader(0,size);

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
		start=explainheader(start,0x1000);

		//next or not
		countbyte += 0x1000;
		if( countbyte > size )break;

	}//while(1)

theend:
	printf(
		"@%x:%d -> %d,%d,%d,%d\n\n\n\n",
		countbyte+start,
		countline,
		instruct,
		inmarco,
		innote,
		instr
	);
	write(dest,"\n\n\n\n",4);
	close(src);
	return;
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
		if(i-j<2)return;

		//长度超过也不对
		if( thisname[ j+2 ] > 0x20 )return;

		//名字不一样不对
		if( strcmp( thisname+j , ".h" ) != 0 )return;

		//文件空的也不对
		i=statbuf.st_size;
		if(i<=0)return;

		//是头文件就进去干
		explainfile(thisname,i);
		return;
	}
}//fileordir




int main(int argc,char *argv[])  
{
	char* out="struct.seed";
	char* in=0;

	//
	if(argc==1)
	{
		in=".";
	}
	if(argc==2)
	{
		in=argv[1];
	}

	//open,process,close
	dest=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	fileordir( in );
	close(dest);
}
