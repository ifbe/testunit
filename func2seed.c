#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 




//destination,source,datahome
static int dest=-1;
static int src=-1;
unsigned char datahome[0x2000];	//4k+4k

//the prophets who guide me
unsigned char* prophet=0;	//后面可能要用的函数名字
unsigned char* prophetinsist=0;	//在函数外面碰到了左括号:
int doubt=0;		//"疑虑"(想更细致就出错):	else myfunc ()

//status
static volatile unsigned int chance=0;
static volatile unsigned int infunc=0;	//在函数内
static volatile unsigned int inmarco=0;	//在宏内
static volatile unsigned int innote=0;	//在注释内
static volatile unsigned int instr=0;	//在字符串内




void printprophet()
{
	unsigned long long temp;

	//if
	temp=*(unsigned short*)prophet;
	if(temp==0x6669)
	{
		if(prophet[2]==' ' | prophet[2]=='(' | prophet[2]==0x9)
		{return;}
	}

	//4ch
	//else
	temp=*(unsigned int*)prophet;
	if(temp==0x65736c65)
	{
		if(prophet[4]==' ' | prophet[4]=='(' | prophet[4]==0x9)
		{return;}
	}

	//3ch
	//for
	temp=0xffffff & (*(unsigned int*)prophet);
	if(temp==0x726f66)
	{
		if(prophet[3]==' ' | prophet[3]=='(' | prophet[3]==0x9)
		{return;}
	}

	//5ch
	//while
	temp=0xffffffffff & (*(unsigned long long*)prophet);
	if(temp==0x656c696877)
	{
		if(prophet[5]==' ' | prophet[5]=='(' | prophet[5]==0x9)
		{return;}
	}

	//6 ch
	//switch , sizeof , printf
	temp=0xffffffffffff & (*(unsigned long long*)prophet);
	if(temp==0x686374697773)
	{
		if(prophet[6]==' ' | prophet[6]=='(' | prophet[6]==0x9)
		{return;}
	}
	else if(temp==0x666f657a6973)
	{
		if(prophet[6]==' ' | prophet[6]=='(' | prophet[6]==0x9)
		{return;}
	}
	//else if(temp==0x66746e697270)
	//{
	//	if(prophet[6]==' ' | prophet[6]=='(' | prophet[6]==0x9)
	//	{return;}
	//}




	//start printing now
	int in=0,out=0;
	char tempbuf[0x80];
	if(infunc > 0)
	{
		tempbuf[0]=0x9;
		out++;
	}
	for(;in<80;in++)
	{
		if(prophet[in]=='(')break;
		if(prophet[in]==' ')break;
		if(prophet[in]==0x9)break;
		if(prophet[in]==0xa)break;
		tempbuf[out]=prophet[in];
		out++;
	}

	//printf(these 6 lines can be deleted)
	tempbuf[out]=0;
	//printf("%s	//%d,%d,%d,%d\n",tempbuf,infunc,inmarco,innote,instr);

	//write
	tempbuf[out]='\n';
	tempbuf[out+1]=0;
	out++;
	write(dest,tempbuf,out);
}
/*
problem4:
		func()
		{

		}
		struct xxxx {
			int (*what)();
		}
		anotherfunc()
		{

		}
*/
int process(int start,int end)
{
	int i=0,j=0;
	unsigned char ch=0;

	//不用i<end防止交界麻烦,给足了整整0x800个机会自己决定滚不滚
	for(i=start;i<0x1800;i++)
	{
		ch=datahome[i];
		if(ch==0)break;

		//printf("%c",ch);
		if( (i>end) && (prophet==0) && (prophetinsist==0))
		{
			if(ch==' ')break;
			else if(ch==0x9)break;
			else if(ch==0xa)break;
			else if(ch==0xd)break;
		}

		//........
		if(ch=='\\')
		{
			i++;
			continue;
		}

		//prophets' guess
		else if(
			( ch>='a' && ch<='z') |
			(ch>='A' && ch<='Z') |
			(ch>='0' && ch<='9') |
			ch=='_' )
		{
			if(inmarco>0|innote>0|instr>0)continue;
			if(prophet==0)prophet=datahome+i;
			else if(doubt==1)
			{
				doubt=0;
				prophet=datahome+i;
			}
		}

		//prophets' doubt
		else if( (ch==' ')|(ch==0x9) )
		{
			if(inmarco>0|innote>0|instr>0)continue;
			if(prophet != 0)doubt=1;
		}

		else if( (ch==0xa)|(ch==0xd) )
		{
			//普通宏的处理方法扔掉这一行,要注意'\\'这个恶心的符号
			if(prophet != 0)doubt=1;
			if(innote==1)innote=0;
		}

		//prophets' fable right or wrong
		else if(ch=='(')
		{
			if(inmarco>0|innote>0|instr>0)continue;
			if(prophet!=0)
			{
				//somthing like:    what=func();
				if(infunc > 0)
				{
					printprophet();
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
			if(inmarco>0|innote>0|instr>0)continue;
			prophet=0;
			doubt=0;

			if(infunc==0)chance=1;
		}
		else if(ch=='{')
		{
			if(inmarco>0|innote>0|instr>0)continue;
			if(infunc==0)
			{
				if( chance > 0 )
				{
					if(prophetinsist!=0)	//消灭aaa=(struct){int a,int b}这种
					{
						prophet=prophetinsist;
						printprophet();
						write(dest,"{\n",2);
						//printf("{//+%x\n",i);

						prophet=0;
						prophetinsist=0;
						doubt=0;

						infunc++;
						chance=0;
					}//insist!=0
				}//chance
			}//infunc
			else
			{
				infunc++;
			}
		}
		else if(ch=='}')
		{
			if(inmarco>0|innote>0|instr>0)continue;
			chance=0;

			if(infunc>0)
			{
				infunc--;

				if(infunc==0)
				{
					write(dest,"}\n",2);
					//printf("}//+%x\n",i);
				}
			}
		}

/*
problem1:	#ifdef xxxx
			if() {
		#else
			if() {
		#endif
*/
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

			//宏里又碰到了#号
			if(inmarco!=0)
			{
				//#else -> 升级
				if( (*(unsigned int*)(datahome+i+1) )==0x65736c65 )
				{

					inmarco=9;
				}

				//#???? -> 降级
				else
				{
					inmarco=0;
				}
			}

			//宏外面碰到#号
			else if(inmarco==0)
			{
				//#if
				if( (*(unsigned short*)(datahome+i+1) )==0x6669 )
				{
					inmarco=1;
				}
			}

/*
			//debug!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			printf("#");
			for(j=1;j<64;j++)
			{
				if(datahome[i+j]<=0xd)break;
				printf("%c",datahome[i+j]);
			}
			printf("\t->(%d,%d,%d,%d)\n",
				infunc,
				inmarco,
				innote,
				instr
			);
*/
		}
/*
problem2:	"{},(),in this......"
problem3:	'{},(),in this......'
*/
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

			//very very very dangerous,don't use loop
			while(1)
			{
				i++;
				if(datahome[i]=='\'')break;
				if(datahome[i]=='\\')i++;
			}
		}

		/*
		else if( (ch=='+') | (ch=='-') )
		{
			prophet=0;
		}
		*/
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
				innote=100;
			}
		}
		else if(datahome[i]=='*')
		{
			if(instr>0)continue;

			if(datahome[i+1]=='/')
			{
				if(innote==100)
				{
					innote=0;
				}
			}
			prophet=0;
			doubt=0;
		}

		//prophets' abandon
		else if(ch==',')
		{
			if(inmarco>0|innote>0|instr>0)continue;
			chance=0;
			doubt=0;
			prophet=0;
		}
		else if( (ch=='=') | (ch==';') | (ch=='&') | (ch=='|') )
		{
			if(inmarco>0|innote>0|instr>0)continue;

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
	char buf[256];
	int ret=0,temp=0;

	//prepare the world
	prophet=prophetinsist=0;
	doubt = chance=0;
	infunc = inmarco = innote = instr = 0;
	src=open(thisfile , O_RDONLY);
	if(src<0){printf("open fail\n");exit(-1);}

	//infomation
	temp=snprintf(buf,256,"#name:	%s\n",thisfile);
	printf("%s",buf);
	write(dest,buf,temp);

	temp=snprintf(buf,256,"#size:	%lld(0x%llx)\n",size,size);
	printf("%s",buf);
	write(dest,buf,temp);

	//<=4k
	if(size<=0x1000)
	{
		//printf("@[%x,%llx):\n",0,size);
		ret=read(src,datahome,size);
		if(ret<0){printf("readfail0\n");exit(-1);}

		datahome[size]=0;	//unknown problem
		process(0,size);

		goto theend;
	}

	//>4k
	int over=0;
	int here=0x1000;
	read(src,datahome+0x1000,0x1000);
	if(ret<0){printf("readfail1\n");exit(-1);}
	while(1)
	{
		//move
		for(temp=0;temp<0x1000;temp++)
		{
			datahome[ temp ] = datahome[ temp+0x1000 ];
		}

		//read or not
		if( here + 0x1000 <= size ) temp=0x1000;	//文件位置+0x1000<总大小
		else temp=size-here;				//马上就结束，或者已经结束一次
		if(temp > 0)
		{
			ret=read(src,datahome+0x1000,temp);
			if(ret<0){printf("readfail2");exit(-1);}
		}

		//处理
		//printf("temp=%x\n",temp);
		if(temp<0)
		{
			temp+=0x1000;		//最后一次
			datahome[temp]=0;		//防止往后走
		}
		else if(temp<0x1000)
		{
			temp=0x1000;	//读已经完了，但是处理的是上一次的!!!!
		}
		//printf("temp=%x\n",temp);

		printf(
			"//start@%x	%d,%d,%d,%d\n",
			here-0x1000+over,
			infunc,
			inmarco,
			innote,
			instr
		);
		over=process(over,temp);
		printf(
			"//end@%x	%d,%d,%d,%d\n",
			here-0x1000+temp+over,
			infunc,
			inmarco,
			innote,
			instr
		);

		//next or not
		here+=0x1000;
		if( here > size+0x1000 )break;

	}//while(1)

theend:
	printf("\n#finish status:%d,%d,%d,%d\n\n\n\n",infunc,inmarco,innote,instr);
	write(dest,"\n\n\n\n",4);
	close(src);
        return;
}
//这里有问题,刚好在[1000,2000]这一半
//但是退出了process函数,于是函数名丢掉了!!!!!!!
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
