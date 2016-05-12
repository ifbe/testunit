#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>



//
unsigned char* buffer=0;
struct stat st;
//
int file=0;
int size;
int bytepersample=0;
int samplepersecond=0;




void pwmplay(int value)
{
	hardwarepwmwrite(3,value);
}




int main(int argc,char** argv)
{
	int ret;
	int sample;

	if(argc==1)
	{
		printf("./a.out 1.wav\n");
		return 0;
	}

	//size
	ret=stat(argv[1],&st);
	if(ret<0)
	{
		printf("no such file or directory\n");
		return 0;
	}
	size=st.st_size;
	printf("size=%d\n",size);

	//malloc
	buffer=(unsigned char*)malloc(size+8);
	if(buffer==NULL)
	{
		printf("no enough memory\n");
		return 0;
	}

	//open
	file=open(argv[1],O_RDONLY);
	if(file<=0)
	{
		printf("error@open\n");
		goto freememory;
	}

	//read
	ret=read(file,buffer,size);
	if(ret!=size)
	{
		printf("error@read\n");
		goto closefile;
	}

	//
	samplepersecond=*(unsigned int*)(buffer+0x18);
	bytepersample = *(unsigned short*)(buffer+0x22);
	bytepersample = bytepersample/8;

	//now it begins
	ret=44;
	while(1)
	{
		if(bytepersample==1)
		{
			sample=buffer[ret];
			ret += 1;

			printf("%x\n",sample);
		}
		else
		{
			sample = buffer[ret] + (buffer[ret+1]<<8);
			ret += 2;

			printf("%x\n",sample);
			pwmplay(sample);
		}

		if(ret >= size)break;
		usleep(22);
	}

closefile:
	close(file);
freememory:
	free(buffer);
}
