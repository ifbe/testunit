#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 




//fp
static int dest=-1;
static int src=-1;

//destination,source,datahome
static unsigned char datahome[0x2000];	//4k+4k
static unsigned char strbuf[256];
static unsigned char backup1[256];
static unsigned char backup2[256];

//the prophets who guide me
static unsigned char* prophet=0;	//后面可能要用的函数名字
static unsigned char* prophetinsist=0;	//在函数外面碰到了左括号:
static int doubt=0;		//"疑虑"(想更细致就出错):	else myfunc ()
static int chance=0;

//count
static int countbyte=0;		//统计字节数
static int countline=0;		//统计行数

//status
static int infunc=0;
	//0:	不在函数里
	//1:	在函数内
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




int copyname(unsigned char* p,unsigned char* q)
{
	int i=0;
	unsigned long long temp;

	//2byte:	if
	temp=*(unsigned short*)p;
	if(temp==0x6669)
	{
		if(p[2]==' ' | p[2]=='(' | p[2]==0x9)
		{
			i=2;
			goto decide;
		}
	}

	//3byte:	for
	temp=0xffffff & (*(unsigned int*)p);
	if(temp==0x726f66)
	{
		if(p[3]==' ' | p[3]=='(' | p[3]==0x9)
		{
			i=3;
			goto decide;
		}
	}

	//4byte:	else
	temp=*(unsigned int*)p;
	if(temp==0x65736c65)
	{
		if(p[4]==' ' | p[4]=='(' | p[4]==0x9)
		{
			i=4;
			goto decide;
		}
	}

	//5byte:	while
	temp=0xffffffffff & (*(unsigned long long*)p);
	if(temp==0x656c696877)
	{
		if(p[5]==' ' | p[5]=='(' | p[5]==0x9)
		{
			i=5;
			goto decide;
		}
	}

	//6byte:	switch , sizeof , printf
	temp=0xffffffffffff & (*(unsigned long long*)p);
	if(temp==0x686374697773)
	{
		if(p[6]==' ' | p[6]=='(' | p[6]==0x9)
		{
			i=6;
			goto decide;
		}
	}
	else if(temp==0x666f657a6973)
	{
		if(p[6]==' ' | p[6]=='(' | p[6]==0x9)
		{
			i=6;
			goto decide;
		}
	}
/*
	else if(temp==0x66746e697270)
	{
		if(p[6]==' ' | p[6]=='(' | p[6]==0x9)
		{
			i=6;
			goto decide;
		}
	}
*/
decide:
	if(i!=0)
	{
		if(infunc != 0)return 0;

		*(unsigned long long*)q=temp;
		*(unsigned int*)(q+i)=0x3f3f3f3f;
		return i+4;
	}

forcecopy:
	for(i=0;i<80;i++)
	{
		if(	((p[i]>='a')&&(p[i]<='z')) |
			((p[i]>='A')&&(p[i]<='Z')) |
			((p[i]>='0')&&(p[i]<='9')) |
			(p[i]=='_') |
			(p[i]=='.') |
			(p[i]=='-') |
			(p[i]=='>') )
		{
			q[i]=p[i];
		}
		else break;
	}

	//0
	q[i]=0;
	return i;

}
void printprophet(unsigned char* p)
{
	int count=0;

	//函数结束
	if(p==0)
	{
		strbuf[0]='}';
		strbuf[1]='\n';
		strbuf[2]=0;
		count=2;
		goto finalprint;
	}

	//在函数外
	if(infunc==0)
	{
		count=copyname(p , strbuf);
		count+=snprintf(
			strbuf+count,
			0x80,
			"	@%d\n{\n",
			countline
		);
	}
	else
	{
		strbuf[0]=0x9;
		count++;

		count += copyname(p , strbuf+1);
		if(count==1)return;

		strbuf[count]='\n';
		strbuf[count+1]=0;
		count++;
	}

finalprint:
	write(dest,strbuf,count);
	//printf("%s",dest);
}
int explainpurec(int start,int end)
{
	int i=0;
	unsigned char ch=0;
	printf(
		"@%x@%d -> %d,%d,%d,%d\n",
		countbyte+start,
		countline+1,
		infunc,
		inmarco,
		innote,
		instr
	);

	//不用i<end防止交界麻烦,给足了整整0x800个机会自己决定滚不滚
	for(i=start;i<0x1800;i++)
	{
		//拿一个
		ch=datahome[i];
		//printf("%c",ch);

		//软退
		if( (i>end) && (prophet==0) && (prophetinsist==0))
		{
			if(ch==' ')break;
			else if(ch==0x9)break;
			else if(ch==0xa)break;
			else if(ch==0xd)break;
		}

		//强退(代码里绝不会有真正的0，都是ascii的0x30)
		if(ch==0)
		{
			//保存一下上次的名字
			if(prophet!=0)
			{
				copyname(prophet,backup1);
				prophet=backup1;
			}
			if(prophetinsist!=0)
			{
				copyname(prophetinsist,backup2);
				prophetinsist=backup2;
			}

			//printf("@%x\n",i);
			break;
		}

		//在这里记录行数？
		else if( (ch==0xa)|(ch==0xd) )
		{
			//
			countline++;

			//换行了，可能函数名不对了
			if(prophet != 0)doubt=1;

			//define宏，换行清零
			if(inmarco=='d')inmarco=0;

			//单行注释，换行清零
			if(innote==1)innote=0;
		}

		//........
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

		//prophets' guess
		else if(
			(ch>='a' && ch<='z') |
			(ch>='A' && ch<='Z') |
			(ch>='0' && ch<='9') |
			ch=='_' )
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			chance=0;

			//
			if(prophet==0)prophet=datahome+i;
			else
			{
				if(doubt==1)
				{
					doubt=0;
					prophet=datahome+i;
				}
			}
		}

		//prophets' doubt
		else if( (ch==' ')|(ch==0x9) )
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			if(prophet != 0)doubt=1;
		}

		//prophets' fable right or wrong
		else if(ch=='(')
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			if(prophet!=0)
			{
				//somthing like:    what=func();
				if(infunc > 0)
				{
					printprophet(prophet);
				}

				//在函数外面碰到了左括号
				else
				{
					prophetinsist=prophet;
				}

				prophet=0;
				doubt=0;
			}
		}
		else if(ch==')')
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			prophet=0;
			doubt=0;

			if(infunc==0)chance=1;
		}
		else if(ch=='{')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			//已经在函数里
			if(infunc!=0)infunc++;

			//确认这即将是个函数
			else
			{
				//消灭aaa=(struct){int a,int b}这种
				if( (chance>0) && (prophetinsist!=0) )
				{
					printprophet(prophetinsist);

					infunc++;
					prophet=prophetinsist=0;
					doubt=chance=0;
				}//chance && insist!=0
			}//infunc
		}
		else if(ch=='}')
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			chance=0;

			if(infunc>0)
			{
				infunc--;
				if(infunc==0)printprophet(0);
			}
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
				//#define
				if( (*(unsigned short*)(datahome+i+1) )==0x6564 )
				{
					if( (*(unsigned int*)(datahome+i+3) )==0x656e6966 )
					{
						inmarco='d';
						i+=6;
					}
				}

				//#if
				else if( (*(unsigned short*)(datahome+i+1) )==0x6669 )
				{
					inmarco=1;
					i+=2;

					//借用一下innote，这一行不能要
					innote=1;
				}

				//#else 这里是为了暂时不管宏嵌套的问题...
				else if( (*(unsigned int*)(datahome+i+1) )==0x65736c65 )
				{

					inmarco='e';
					i+=4;
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

				//#endif -> 降级
				else if( (datahome[i+1]=='e') &&
				    (datahome[i+2]=='n') &&
				    (datahome[i+3]=='d') &&
				    (datahome[i+4]=='i') &&
				    (datahome[i+5]=='f') )
				{
					inmarco=0;
					i+=5;
				}
			}

			//else里面碰到endif号
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
			if(datahome[i+1]=='/')	//    //
			{
				innote=1;
			}

			//多行注释
			else if(datahome[i+1]=='*')	//    /*
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
			prophet=0;
			doubt=0;
		}

		else if(ch==',')
		{
			if(inmarco>=2|innote>0|instr>0)continue;
			chance=0;
			doubt=0;
			prophet=0;
		}

		else if(ch=='&')
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			doubt=0;
			prophet=0;
			prophetinsist=0;
		}
		else if( (ch=='=') | (ch==';') | (ch=='|') )
		{
			if(inmarco>=2|innote>0|instr>0)continue;

			chance=0;
			doubt=0;
			prophet=0;
			prophetinsist=0;
		}

		else if(datahome[i]>0x80)		//utf-8 > 0x80
		{
			//do nothing
		}

	}//for

	return i-end;	//可能多分析了几十几百个字节
}
void explainfile(char* thisfile,unsigned long long size)
{
	int start=0;
	int end=0;
	int ret=0;

	//init
	prophet=prophetinsist=0;
	doubt = chance=0;
	countbyte=countline=0;
	infunc = inmarco = innote = instr = 0;

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

theend:
	printf("@%x@%d -> %d,%d,%d,%d\n\n\n\n",
		countbyte+start,
		countline,
		infunc,
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
		if( strcmp( thisname+j , ".c" ) != 0 )return;

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
	char* in=0;
	char* out="func.seed";
	if(argc==1)
	{
		in=".";
	}
	else if(argc==2)
	{
		in=argv[1];
	}

	//open,process,close
	dest=open(out,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	fileordir( in );
	close(dest);
}
