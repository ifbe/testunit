#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

static int fp=0;
int systemi2c_write(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
        int ret;

        //select device
        ret=ioctl(fp,I2C_SLAVE,dev);
        if(ret < 0)
        {
                printf("error@systemi2c_write.ioctl\n");
                return;
        }

        //select register
        ret=write(fp,&reg,1);
        if(ret!=1)
        {
                printf("error@systemi2c_write.writereg:%x\n",ret);
                return -1;
        }

        //write data
        ret=write(fp,buf,count);
        if(ret<=0)
        {
                printf("error@systemi2c_write.writedata:%x\n",ret);
                return -2;
        }
        return 1;
}
int systemi2c_read(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
        int ret;

        //select device
        ret=ioctl(fp,I2C_SLAVE,dev);
        if(ret < 0)
        {
                printf("error@systemi2c_read.ioctl\n");
                return;
        }

        //select register
        ret=write(fp,&reg,1);
        if(ret!=1)
        {
                printf("error@systemi2c_read.writereg:%x\n",ret);
                return -1;
        }

        //read data
        ret=read(fp,buf,count);
        if(ret<=0)
        {
                printf("error@systemi2c_read.readdata:%x\n",ret);
                return -2;
        }
        return 1;
}

int initlibrary()
{
	int i=0;

        fp = open("/dev/i2c-1",O_RDWR);
        if(fp<0)
        {
                printf("error open\n");
                return;
        }

	return 1;
}
void killlibrary()
{
	close(fp);
}
