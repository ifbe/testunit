#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 




//
static int dest=-1;
static int src=-1;

//
static unsigned char datahome[0x2000];	//4k+4k
static unsigned char strbuf[256];
static unsigned char backup[256];

//
static char* prophet=0;
static char* a[16];

//
static int countbyte=0;		//统计字节数
static int countline=0;		//统计行数

//
static int instruct=0;
	//0:	不在结构体里
	//1:	在结构体内
	//?:	被包在第几个括号里
static int inmarco=0;
	//0:	不在宏里
	//1:	在普通宏内
	//'d':	#define
	//'e':	#else
static int innote=0;
	//0:	不在注释里
	//1:	//
	//9:	/**/
static int instr=0;
	//0:	不在字符串里
	//1:	在字符串内




int pickname(unsigned char* p,unsigned char* q)
{
	int i=0;
	int o=0;

	a[0]=a[1]=0;

	//eat
	for(i=0;i<0x100;i++)
	{
		if(p[i]==';')break;
		else if(p[i]=='{')break;

		//碰到空格
		else if( (p[i]==0x9) | (p[i]==0x20) )
		{
			//手上有东西 -> 这个结束了
			if(a[o]!=0)
			{
				o++;
				a[o]=0;
			}
		}

		//可用字符
		else
		{
			//第一个字节 -> 记录位置
			if(a[o]==0)
			{
				a[o]=p+i;
			}
		}
	}

	o=0;
	if(a[1]!=0)p=a[1];
	for(i=0;i<0x100;i++)
	{
		if(     ((p[i]>='a')&&(p[i]<='z')) |
			((p[i]>='A')&&(p[i]<='Z')) |
			((p[i]>='0')&&(p[i]<='9')) |
			(p[i]=='_') )
		{
			q[o]=p[i];
			o++;
		}
		else break;
	}
	return o;
}
int printprophet(unsigned char* p)
{
	int i,o;

	//结构体结束
	if(p==0)
	{
		//o=snprintf(strbuf,0x80,"}%d,%d,%d,%d\n",instruct,inmarco,innote,instr);
		o=snprintf(strbuf,0x80,"}\n");
		goto printthis;
	}

	//新结构体
	if(instruct==0)
	{
		o  = pickname(p , strbuf);
		o += snprintf(strbuf+o,0x80,"	@%d\n{\n",countline+1);
	}

	//结构体内部结构体
	else
	{
		o=1;
		strbuf[0]=0x9;
		o += pickname(p , strbuf+1);
		o += snprintf(strbuf+o,0x80,"\n",countline+1);
		//o += snprintf(strbuf+o,0x80,"	@%d\n",countline+1);
	}

printthis:
	//printf("%s",strbuf);
	write(dest,strbuf,o);
	prophet=0;
	return 0;
}
int checkprophet(unsigned char* p)
{
	int i;

	//struct
	if( p[0] == 's' )
	{
		if( p[1] != 't' )goto wrong;
		if( p[2] != 'r' )goto wrong;
		if( p[3] != 'u' )goto wrong;
		if( p[4] != 'c' )goto wrong;
		if( p[5] != 't' )goto wrong;
		i=6;
	}

/*
	//union
	else if(p[0]=='u')
	{
		if( p[1] != 'n' )goto wrong;
		if( p[2] != 'i' )goto wrong;
		if( p[3] != 'o' )goto wrong;
		if( p[4] != 'n' )goto wrong;
		i=5;
	}
	//enum
	else if(p[0]=='e')
	{
		if( p[1] != 'n' )goto wrong;
		if( p[2] != 'u' )goto wrong;
		if( p[3] != 'm' )goto wrong;
		i=4;
	}
*/
	//not this
	else goto wrong;

	//解决"nameisstructisname"的问题
	if(	(p[i] != 0x20) &&
		(p[i] !=  0x9) &&
		(p[i] !=  '{') &&
		(p[i] !=  0xa) &&
		(p[i] !=  0xd) )
	{
		goto wrong;
	}

correct:
//printf("correct:%c\n",p[0]);
	return i-1;

wrong:
	prophet=0;
//printf("wrong:%c\n",p[0]);
	return 0;
}
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
		if( (i>end) && (prophet==0) && (instruct==0) )
		{
			if(ch==' ')break;
			else if(ch==0x9)break;
			else if(ch==0xa)break;
			else if(ch==0xd)break;
		}

		//强退(代码里绝不会有真正的0，都是ascii的0x30)
		if(ch==0)
		{
			if(prophet!=0)
			{
				pickname(prophet,backup);
				prophet=backup;
			}

                        //printf("@%x\n",i);
                        break;
                }

		//在这里记录行数？
		else if( (ch==0xa)|(ch==0xd) )
		{
			//
			countline++;

			//define宏，换行清零
			if(inmarco=='d')inmarco=0;

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

		//这里有bug，暂时不管字符
		else if(ch=='\'')
		{
			if(innote>0|instr>0)continue;

			if(datahome[i+2]=='\'')
			{
				i+=2;
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
				i++;
                        }

                        //多行注释
                        else if(datahome[i+1]=='*')     //    /*
                        {
                                innote=9;
				i++;
                        }
                }
                else if(datahome[i]=='*')
                {
                        if((innote==1)|(instr>0))continue;

                        if(datahome[i+1]=='/')
                        {
                                if(innote==9)
                                {
                                        innote=0;
					i++;
                                }
                        }
                }
		else if(ch=='s')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			//
			if(prophet==0)
			{
				prophet=datahome+i;
				i += checkprophet( prophet );
			}
		}
/*
		else if(ch=='u')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			//
			if(prophet==0)
			{
				prophet=datahome+i;
				i += checkprophet( prophet );
			}
		}
*/
		else if(ch==';')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			if(prophet!=0)
			{
				if(instruct>0)
				{
					printprophet(prophet);
				}
				prophet=0;
			}
		}

		else if(ch=='{')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			if(prophet!=0)
			{
				printprophet(prophet);
				instruct++;
			}
		}

		else if(ch=='}')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			if(instruct>1)instruct--;
			else if(instruct==1)
			{
				instruct=0;
				printprophet(0);
			}
		}

		else if(ch==')')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			if(prophet!=0)
			{
				prophet=0;
			}
		}

		else if(ch=='>')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			if(prophet!=0)prophet=0;
		}

		else if(ch=='#')
		{
                        //不在注释里面,也不在字符串里的时候
                        if(innote>0|instr>0)continue;

                        //吃掉所有空格和tab
                        while(1)
                        {
                                if( (datahome[i+1]==' ') | (datahome[i+1]==0x9) )i++;
                                else break;
                        }

                        //宏外面碰到#号
                        if(inmarco==0)
                        {
                                //#if
                                if( (*(unsigned short*)(datahome+i+1) )==0x6669 )
                                {
                                        inmarco=1;
                                        i+=2;
                                }

                                //#define
                                if( (*(unsigned short*)(datahome+i+1) )==0x6564 )
                                {
                                        if( (*(unsigned int*)(datahome+i+3) )==0x656e6966 )
                                        {
                                                inmarco='d';
                                                i+=6;
                                        }
                                }
                        }

                        //普通宏里又碰到了#号
                        else if(inmarco==1)
                        {
                                //#else -> 升级
                                if( (*(unsigned int*)(datahome+i+1) )==0x65736c65 )
                                {

                                        inmarco='e';
                                        i+=4;
                                }
                        }

                        //else里面碰到#号
                        else if(inmarco=='e')
                        {
                                if( (datahome[i+1]=='e') &&
                                    (datahome[i+2]=='n') &&
                                    (datahome[i+3]=='d') &&
                                    (datahome[i+4]=='i') &&
                                    (datahome[i+5]=='f') )
                                {
                                        inmarco=0;
                                        i+=5;
                                }
                        }
		}//#
	}//for

	return i-end;
}
void explainfile(char* thisfile,unsigned long long size)
{
	int start=0;
	int end=0;
	int ret=0;

	//init
	prophet=0;
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
