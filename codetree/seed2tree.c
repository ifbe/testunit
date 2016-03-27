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
static char infile[256]={0};
	//输入文件名
static char outfile[256]={0};
	//输出文件名
static char funcname[256]={0};
static int  funclength=0;
	//请求的函数名




//在已经读完的缓冲区里搜索自己
//搜不到就写上等级(tab个数)，以及名称(字符串)，外加问号
//搜到就递归调用这个函数(省的手动压栈)
static char tempforname[256];
static int namestack[16];
static int depth=0;
void nodeorleaf
(
	char* start,int count,		//search where , byte count
	char* wantname,int namesize	//name addr , name count
)
{
	int new=1;	//isnewline
	int this=0,run=0;

	//try to find it!
	while(1)
	{
		if(new)
		{
			//不一样就提前跳出来
			for(run=0;run<namesize;run++)
			{
				if(start[this+run] != wantname[run])break;
			}

			//没有提前跳出来说明找到了那就跳出while(1)
			if( run==namesize )
			{
				//必须再确认一个东西,防止printf找到printfxxxx
				if(start[this+run]=='\n')break;
				if(start[this+run]==0x9)break;
				if(start[this+run]==0x20)break;
			}

			//继续往下搜索
			else new=0;
		}
		else		//old
		{
			if(start[this]=='\n')
			{
				new=1;
			}
		}

		//无论if或者else都得到这里进行下一次检查
		this++;
		if( this >= count )break;
	}
	//不管怎样，进来了都要打上名字
	for(run=0;run<depth;run++)
	{
		printf("	");
		write(dest,"	",1);
	}
	for(run=0;run<namesize;run++)
	{
		printf( "%c" , wantname[ run ] );
		write( dest , wantname+run , 1 );
	}




	//1.第1种：找不到了
	if( this >= count )
	{
		printf("\n");
		write(dest,"\n",1);
		return;
	}

	//2.第3种：栈太深不敢进
	if(depth>=8)
	{
		printf("	#too deep!\n");
		write(dest,"	#too deep!\n",12);
		return;
	}

	//3.第1种：判断递归
	if( depth>0)
	{
		for(run=0;run<depth;run++)
		{
			if( namestack[run]==this )
			{
				printf("	#recursion\n");
				write(dest,"	#recursion\n",12);
				return;
			}
		}
	}




	//左括号
	printf("	@%x\n" , this );
	write(dest,"\n",1);
	for(run=0;run<depth;run++)
	{
		printf("	");
		write(dest,"	",1);
	}
	printf("{\n");
	write(dest,"{\n",2);

	//进去
	run=this;
	while(1)
	{
		if( start[run] == '}' )break;
		if( start[run] != 0xa )
		{
			run++;
			continue;
		}
		if( start[run+1] !=0x9 )
		{
			run++;
			continue;
		}

		run+=2;
		new=0;
		while(start[run+new]!='\n')new++;

		namestack[depth]=this;
		depth++;
		nodeorleaf(start,count,start+run,new);
		depth--;

		run+=new;
	}

	//右括号
	for(run=0;run<depth;run++)
	{
		printf("	");
		write(dest,"	",1);
	}
	printf("}\n");
	write(dest,"}\n",2);
}
void seed2tree(char* inininin)
{
	struct stat	statbuf;
	char* buf;
	int temp;
	printf("%s\n",inininin);

	//check
	temp=stat( inininin , &statbuf );
	if(temp == -1)
	{
		printf("wrong file\n");
		goto statfailed;
	}

	//open
	src=open(inininin , O_RDONLY|O_BINARY);
	if(src<0)
	{
		printf("open failed\n");
		goto openfailed;
	}

	//malloc
	buf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(buf==NULL)
	{
		printf("too large\n");
		goto mallocfailed;
	}

	//read
	temp=read(src,buf,statbuf.st_size);
	if(temp!=statbuf.st_size)
	{
		printf("read failed\n");
		goto readfailed;
	}

	//process
	depth=0;
	namestack[0]=0;
	nodeorleaf( buf , statbuf.st_size , funcname , funclength );
/*
	for( temp=0;temp<statbuf.st_size;temp++ )
	{
		printf("%c",buf[temp]);
	}
*/

readfailed:
	free(buf);
mallocfailed:
	close(src);
openfailed:
	return;
statfailed:
	return;
}




int main(int argc,char *argv[])  
{
	//
	int i;
	char* p;
	infile[0]=outfile[0]=funcname[0]=0;




	//************************help*************************
	if(argc==1)
	{
		printf("usage:\n");
		printf("seed2tree name\n");
		printf("seed2tree func=main\n");
		printf("seed2tree func=the_func_name infile=code.seed outfile=code.tree\n");
		return 0;
	}
	//******************************************************




	//********************分析输入开始*********************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//func
		if( (p[0]=='f') &&
			(p[1]=='u') &&
			(p[2]=='n') &&
			(p[3]=='c') &&
			(p[4]=='=') )
		{
			printf("infile=%s\n",p+5);
			snprintf(funcname,16,"%s",p+5);
			funclength=strlen(funcname);
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
	if(funcname[0]==0)
	{
		printf("invalid funcname,using main\n");
		snprintf(funcname,16,"main");
		funclength=strlen(funcname);
	}
	if(infile[0]==0)
	{
		printf("invalid infile,using code.seed\n");
		snprintf(infile,16,"code.seed");
	}
	if(outfile[0]==0)
	{
		printf("invalid outfile,using code.tree\n");
		snprintf(outfile,16,"code.tree");
	}
	//********************检查结束**********************




	//open,process,close
	dest=open(outfile,O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	seed2tree(infile);
	close(dest);
}
