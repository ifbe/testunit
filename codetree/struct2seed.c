#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <sys/stat.h>  
#include <sys/types.h> 




//
int dest=-1;
//
char* wantedsuffix=".h";
int wantedlength=2;




void explainfile(char* thisfile,unsigned long long size)
{
	char buf[256];
	int ret=0,temp=0;

	//infomation
	temp=snprintf(buf,256,"#name:	%s\n",thisfile);
	printf("%s",buf);
	write(dest,buf,temp);

	temp=snprintf(buf,256,"#size:	%lld(0x%llx)\n",size,size);
	printf("%s",buf);
	write(dest,buf,temp);
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
		if(i-j<wantedlength)return;

		//长度超过也不对
		if( thisname[ j+wantedlength ] > 0x20 )return;

		//名字不一样不对
		if( strcmp( thisname+j , wantedsuffix ) != 0 )return;

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
	if(argc==3)
	{
		wantedsuffix=argv[2];
		wantedlength=strlen(argv[2]);
	}

	//open,process,close
	dest=open("struct.seed",O_CREAT|O_RDWR|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
	fileordir( argv[1] );
	close(dest);
}
