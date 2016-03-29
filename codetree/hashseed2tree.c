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




//hash
static char hashname[256]={0};
static int hashfd=-1;
static char* hashbuf=0;
static int hashcount=0;
//seed
static char seedname[256]={0};
static int seedfd=-1;
static char* seedbuf=0;
static int seedcount=0;
//tree
static char treename[256]={0};
static int treefd=-1;
static int stack[16]={0};
static int depth=0;
//
static unsigned char strbuf[256]={0};
static struct stat statbuf;
//
static unsigned int badhash=0;
static unsigned int goodhash=0;




void readall()
{
	int ret=0;




//************************hash***************************
	//stat
	ret=stat( hashname , &statbuf );
	if(ret == -1)
	{
		printf("(error)hash name\n");
		exit(-1);
	}
	if(statbuf.st_size<16)
	{
		printf("(error)hash size1\n");
		exit(-1);
	}
	if( (statbuf.st_size%16) != 0 )
	{
		printf("(error)hash size2\n");
		exit(-1);
	}

	//malloc
	hashbuf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(hashbuf==NULL)
	{
		printf("(error)hash malloc\n");
		exit(-1);
	}

	//open
	hashfd=open(hashname , O_RDONLY|O_BINARY);
	if(hashfd<0)
	{
		printf("(error)hash open\n");
		free(hashbuf);
		exit(-1);
	}

	//read
	hashcount=read(hashfd,hashbuf,statbuf.st_size);
	if(hashcount!=statbuf.st_size)
	{
		printf("(error)hash read\n");
		close(hashfd);
		free(hashbuf);
		exit(-1);
	}

	printf("%s has %d functions\n",hashname,hashcount>>4);
	close(hashfd);
//********************************************************




//***********************seed*****************************
	//stat
	ret=stat( seedname , &statbuf );
	if(ret == -1)
	{
		printf("(error)seed name\n");
		exit(-1);
	}
	if(statbuf.st_size<16)
	{
		printf("(error)seed size\n");
		exit(-1);
	}

	//malloc
	seedbuf=(char*)malloc( (statbuf.st_size) + 0x1000 );
	if(seedbuf==NULL)
	{
		printf("(error)seed malloc\n");
		exit(-1);
	}

	//open
	seedfd=open(seedname , O_RDONLY|O_BINARY);
	if(seedfd<0)
	{
		printf("(error)seed open\n");
		free(seedbuf);
		exit(-1);
	}

	//read
	seedcount=read(seedfd,seedbuf,statbuf.st_size);
	if(seedcount!=statbuf.st_size)
	{
		printf("(error)read\n");
		close(seedfd);
		free(seedbuf);
		exit(-1);
	}

	printf("%s has %d bytes\n",seedname,seedcount);
	close(seedfd);
//********************************************************
}




int searchhash(char* p , int size)
{
	int i;
	int min;
	int max;
	unsigned int temp1;

	//first,generate the hash value
	badhash=0;
	goodhash=5381;
	for(i=0;i<size;i++)
	{
		badhash=badhash+p[i];
		goodhash=(goodhash<<5)+goodhash+p[i];
	}
	badhash=(badhash&0xffff)+(size<<16);
	//printf("badhash=%x,goodhash=%x\n",badhash,goodhash);

	//second,search for the hash value
	min=0;
	max=hashcount/16;
	while(1)
	{
		//printf("trying:(%x,%x)\n",min,max);
		if(max-min<=0)
		{
			//printf("not found\n");
			return -1;
		}

		i=(min+max)/2;
		//printf("i=%x\n",i);

		temp1=*(unsigned int*)(hashbuf + i*16 + 0xc);
		//printf("temp1=%x\n",temp1);

		if(temp1 == goodhash)
		{
			//printf("found@%d\n",*(unsigned int*)(hashbuf+i*16));
			//break;
			return i*16;
		}
		else if(temp1 > goodhash)
		{
			max=i-1;
		}
		else
		{
			min=i+1;
		}
	}//while(1)
}




void printhash(int where)
{
	int i;
	int min;
	int max;
	unsigned int temp1;
	unsigned int temp2;
	unsigned int temp3;
	unsigned int temp4;

	min=max=where;
	while(1)
	{
		temp1=*(unsigned int*)(hashbuf + min - 0x4);
		if(temp1 != goodhash)break;

		min=min-16;
	}
	while(1)
	{
		temp1=*(unsigned int*)(hashbuf + max + 0x1c);
		if(temp1 != goodhash)break;

		max=max+16;
	}
	for(i=min;i<=max;i+=16)
	{
		temp3=*(unsigned int*)(hashbuf+i+8);
		if(badhash!=temp3)
		{
			printf("conflict hash!\n");
			continue;
		}

		temp1=*(unsigned int*)(hashbuf+i+0x0);
		temp2=*(unsigned int*)(hashbuf+i+0x4);
		temp4=*(unsigned int*)(hashbuf+i+0xc);
		printf("(%.8d,%.8x,%.8x,%.8x)\n",temp1,temp2,temp3,temp4);
	}

	//file
	i=*(unsigned int*)(hashbuf+where+4);
	while(1)
	{
		if(	(seedbuf[i]=='#') &&
			(seedbuf[i+1]=='n') &&
			(seedbuf[i+2]=='a') &&
			(seedbuf[i+3]=='m') &&
			(seedbuf[i+4]=='e') )
		{
			i+=7;
			break;
		}

		i--;
		if(i==0)break;
	}
	while(1)
	{
		printf("%c",seedbuf[i]);
		if(seedbuf[i] == 0xa)break;
		i++;
	}
	
	//function
	i=*(unsigned int*)(hashbuf+where+4);
	while(1)
	{
		printf("%c",seedbuf[i]);
		if(seedbuf[i]=='}')break;

		i++;
	}
	printf("\n");
}




void createtree(char* p,int sz)
{
	int i;
	int this;
	int where;
	int len;




	//找不到就原样打印
	this=searchhash(p,sz);
	if(this==-1)
	{
		//打tab
		for(i=0;i<depth;i++)
		{
			write(treefd,"	",1);
		}

		write(treefd,p,sz);
		write(treefd,"\n",1);
		return;
	}




	//找到了就进去
	len=*(unsigned short*)(hashbuf+this+0xa);
	where=*(unsigned int*)(hashbuf+this+4);




	//打tab,打函数名
	for(i=0;i<depth;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,seedbuf+where,len);




	//检查栈深，递归，重复等等情况，打出原因并且退出
	if(depth>8)
	{
		write(treefd,"	#too deep\n",11);
		return;
	}
	for(i=0;i<depth;i++)
	{
		if(stack[i]==this)
		{
			write(treefd,"	#recursion\n",12);
			return;
		}
	}




	//过了检查就打个换行
	write(treefd,"\n",1);




	//打tab,打左括号
	for(i=0;i<depth;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,"{\n",2);
	stack[depth]=this;
	depth++;




	//开始递归
	while(1)
	{
		if(seedbuf[where]=='{')break;
		where++;
	}
	while(1)
	{
		if(seedbuf[where]=='}')break;
		if(seedbuf[where]==0xa)
		{
			if(seedbuf[where+1]==0x9)
			{
				where+=2;
				len=0;
				while(1)
				{
					if(seedbuf[where+len]==0xa)break;
					len++;
				}

				createtree(seedbuf+where,len);
			}
		}
		where++;
	}




	//打tab,打括号
	depth--;
	for(i=0;i<depth;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,"}\n",2);
}




int main(int argc,char *argv[])  
{
	//
	int i;
	char* p;
	hashname[0]=seedname[0]=treename[0]=0;




	//************************help*************************
/*
	if(argc==1)
	{
		printf("usage:\n");
		printf("hash2tree seed=code.hash outfile=code.tree\n");
		return 0;
	}
*/
	//******************************************************




	//*****************************************
	for(i=1;i<argc;i++)
	{
		p=argv[i];
		if(p==0)break;

		//hash=
		if(	(p[0]=='h') &&
			(p[1]=='a') &&
			(p[2]=='s') &&
			(p[3]=='h') &&
			(p[4]=='=') )
		{
			printf("hash=%s\n",p+7);
			snprintf(hashname,256,"%s",p+7);
		}

		//seed=
		else if((p[0]=='s') &&
			(p[1]=='e') &&
			(p[2]=='e') &&
			(p[3]=='d') &&
			(p[4]=='=') )
		{
			printf("seed=%s\n",p+7);
			snprintf(seedname,256,"%s",p+7);
		}

		//tree=
		else if((p[0]=='t') &&
			(p[1]=='r') &&
			(p[2]=='e') &&
			(p[3]=='e') &&
			(p[4]=='=') )
		{
			printf("tree=%s\n",p+8);
			snprintf(treename,256,"%s",p+8);
		}

		//default treat as wanted name
		else
		{
			snprintf(strbuf,256,"%s",p);
		}
	}
	//*************************************




	//****************************************
	//default......................
	if(hashname[0]==0)
	{
		//printf("hash=code.hash\n");
		snprintf(hashname,256,"code.hash");
	}
	if(seedname[0]==0)
	{
		//printf("seed=code.seed\n");
		snprintf(seedname,256,"code.seed");
	}

	//read.........................
	readall();
	//******************************************




	if(strbuf[0]==0)
	{
		while(1)
		{
			fgets(strbuf,256,stdin);
			//printf("fgets:%s\n",strbuf);

			i=searchhash( strbuf , strlen(strbuf)-1 );
			if(i != -1)printhash(i);
		}
	}
	else
	{
		snprintf(treename,256,"%s.tree",strbuf);
		treefd=open(
			treename,
			O_CREAT|O_RDWR|O_TRUNC|O_BINARY,
			S_IRWXU|S_IRWXG|S_IRWXO
		);

		//递归找函数
		createtree( strbuf , strlen(strbuf) );

		write(treefd,"\n",1);
		close(treefd);
	}
}
