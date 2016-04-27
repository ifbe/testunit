#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>




static unsigned char buf[256];
void explainraw(unsigned char* buf,int size)
{
        int x,y;
        printf("report{\n");
        for(y=0;y<size/16;y++)
        {
                for(x=0;x<size%16;x++)
                {
                        printf("%.2x ",buf[(y*16)+x]);
                }
                printf("\n");
        }
        printf("}\n");
}
void explaintouch(unsigned char* buf,int size)
{
        int x,y;
        if(buf[1]==0)printf("touch end\n");
        else
        {
                x=(buf[2]<<8)+buf[3];
                y=(buf[4]<<8)+buf[5];
                printf("(%d,%d)\n",x,y);
        }
}
void main(int argc,char** argv)
{
        int ret;
        int dev;
	int method=0;

        if(argc==1)
        {
                printf("example:\n");
                printf("./a.out /dev/hidraw0\n");
                printf("./a.out /dev/hidraw0 touch\n");
                return;
        }
	else if(argc==2)
	{
	}
	else if(argc==3)
	{
		method=1;
	}

        dev=open(argv[1],O_RDONLY);
        if(dev==-1)
        {
                printf("open error\n");
                return;
        }

        while(1)
        {
		//read
                ret=read(dev,buf,256);

		//
		if(method==0)explainraw(buf,ret);
                else if(method==1)explaintouch(buf,ret);
	}
}
