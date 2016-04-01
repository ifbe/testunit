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
static unsigned char* hashbuf=0;
static int hashcount=0;
//seed
static char seedname[256]={0};
static int seedfd=-1;
static unsigned char* seedbuf=0;
static int seedcount=0;
//tree
static char treename[256]={0};
static int treefd=-1;
static unsigned char strbuf[256]={0};
static struct stat statbuf;
//stack
static int stack[16]={0};
static int depthnow=0;
static int depthmax=8;
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
	unsigned int temp;




	//1,generate the hash value
	badhash=0;
	goodhash=5381;
	for(i=0;i<size;i++)
	{
		badhash=badhash+p[i];
		goodhash=(goodhash<<5)+goodhash+p[i];
	}
	badhash=(badhash&0xffff)+(size<<16);
	//printf("badhash=%x,goodhash=%x\n",badhash,goodhash);




	//2,bisection search for "goodhash"
	min=0;
	max=hashcount/16;
	while(1)
	{
		//printf("trying:(%x,%x)\n",min,max);
		if(max-min<0)
		{
			//printf("not found\n");
			return -1;
		}
		i=(min+max)/2;

		//check
		temp=*(unsigned int*)(hashbuf + (i*16) + 0xc);
		if(temp == goodhash)
		{
			temp=*(unsigned int*)(hashbuf + (i*16) + 0x8);
			if(temp == badhash)return i*16;
			else break;
		}
		else if(temp > goodhash)
		{
			max=i-1;
		}
		else
		{
			min=i+1;
		}
	}//while(1)




	//3."badhash" not match,go left
	min=i<<4;
	while(1)
	{
		min-=16;
		if(min<0)break;

		temp=*(unsigned int*)(hashbuf + min + 0xc);
		if(temp != goodhash)continue;

		temp=*(unsigned int*)(hashbuf + min + 0x8);
		if(temp == badhash)return min;
	}

	//4."badhash" not match,go right
	max=i<<4;
	while(1)
	{
		max+=16;
		if(max > hashcount)break;

		temp=*(unsigned int*)(hashbuf + min + 0xc);
		if(temp != goodhash)continue;

		temp=*(unsigned int*)(hashbuf + min + 0x8);
		if(temp == badhash)return max;
	}

	//not found
	return -1;
}




void createtree(char* p,int sz)
{
	int i;
	int this;
	int where;
	int len;
	unsigned long long temp1;
	unsigned long long temp2;
	unsigned long long temp3;




	//打tab,打函数名
	for(i=0;i<depthnow;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,p,sz);




	//1.found nothing
	this=searchhash(p,sz);
	if(this==-1)
	{
		write(treefd,"\n",1);
		return;
	}

	//2.found more than one
	if(this==0)temp1=0;
	else temp1=*(unsigned long long*)(hashbuf+this- 0x8);
	temp2=*(unsigned long long*)(hashbuf+this+ 0x8);
	temp3=*(unsigned long long*)(hashbuf+this+0x18);

	if( (temp1 == temp2)|(temp2 == temp3) )
	{
		write(treefd,"	#dumplicate\n",13);
		return;
	}

	//3.too deep
	if(depthnow>=depthmax)
	{
		write(treefd,"	#too deep\n",11);
		return;
	}

	//4.recursion
	for(i=0;i<depthnow;i++)
	{
		if(stack[i]==this)
		{
			write(treefd,"	#recursion\n",12);
			return;
		}
	}

	//ok.过了检查就打个换行
	write(treefd,"\n",1);




	//打tab,打左括号
	for(i=0;i<depthnow;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,"{\n",2);
	stack[depthnow]=this;
	depthnow++;




	//找到了就进去,开始递归
	where=*(unsigned int*)(hashbuf+this+4);
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
	depthnow--;
	for(i=0;i<depthnow;i++)
	{
		write(treefd,"	",1);
	}
	write(treefd,"}\n",2);
}




void printhash(int where)
{
	int ii;
	int jj;
	int min;
	int max;
	unsigned int temp1;
	unsigned int temp2;
	unsigned int temp3;
	unsigned int temp4;

	//min
	if(where==0)min=0;
	else
	{
		min=where;
		while(1)
		{
			//前面一个
			temp1=*(unsigned int*)(hashbuf + min - 0x4);
			if(temp1 != goodhash)break;

			//
			temp2=*(unsigned int*)(hashbuf + min - 0x8);
			if(temp2 != badhash)break;

			//
			min=min-16;
			if(min<=0)break;
		}
	}

	//max
	max=where;
	while(1)
	{
		//后面一个
		temp1=*(unsigned int*)(hashbuf + max + 0x1c);
		if(temp1 != goodhash)break;

		temp2=*(unsigned int*)(hashbuf + max + 0x18);
		if(temp2 != badhash)break;

		max=max+16;
	}

	//only one , print the tree?
	if(min==max)
	{
		//prepare
		treefd=1;
		depthmax=4;

		//打印1：函数所在文件名
		jj=*(unsigned int*)(hashbuf+where+4);
		while(1)
		{
			if(	(seedbuf[jj]=='#') &&
				(seedbuf[jj+1]=='n') &&
				(seedbuf[jj+2]=='a') &&
				(seedbuf[jj+3]=='m') &&
				(seedbuf[jj+4]=='e') )
			{
				jj+=7;
				break;
			}

			jj--;
			if(jj==0)break;
		}
		ii=jj;
		while(1)
		{
			if(seedbuf[ii] == 0xa)break;
			ii++;
		}
		write(treefd,seedbuf+jj,ii-jj+1);

		//body
		temp1=*(unsigned int*)(hashbuf+where+4);
		temp2=*(unsigned short*)(hashbuf+where+0xa);
		createtree(seedbuf+temp1,temp2);
		return;
	}

	//printall
	for(ii=min;ii<=max;ii+=16)
	{
		//打印1：函数所在文件名
		jj=*(unsigned int*)(hashbuf+ii+4);
		while(1)
		{
			if(	(seedbuf[jj]=='#') &&
				(seedbuf[jj+1]=='n') &&
				(seedbuf[jj+2]=='a') &&
				(seedbuf[jj+3]=='m') &&
				(seedbuf[jj+4]=='e') )
			{
				jj+=7;
				break;
			}

			jj--;
			if(jj==0)break;
		}
		while(1)
		{
			printf("%c",seedbuf[jj]);
			if(seedbuf[jj] == 0xa)break;
			jj++;
		}




		//打印2：整个函数
		jj=*(unsigned int*)(hashbuf+ii+4);
		while(1)
		{
			//打印到右括号为止
			printf("%c",seedbuf[jj]);
			if(seedbuf[jj]=='}')break;

			jj++;
		}
		printf("\n");
	}
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

		//depth=
		if((p[0]=='d') &&
			(p[1]=='e') &&
			(p[2]=='p') &&
			(p[3]=='t') &&
			(p[4]=='h') &&
			(p[5]=='=') )
		{
			printf("depth=%s\n",p+6);
			depthmax=atoi(p+6);
		}

		//hash=
		else if((p[0]=='h') &&
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
			if(i == -1)
			{
				printf("not found\n");
			}
			else printhash(i);

			//必须加这个，不然管道有大问题
			fflush(stdout);
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
