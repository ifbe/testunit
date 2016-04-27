#define BYTE unsigned char
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>
int fp;
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
        //usleep(100);

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
                printf("error@systemi2c_read.writereg:%x\n",ret);                return -1;
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

/*
int readi2c(int device,unsigned char reg,unsigned char* mem,int count)
{
        //write address
        int ret=write(fp,&reg,1);
        if(ret!=1)
        {
                //printf("(error write)%x:%x\n",reg,ret);
                return -1;
        }
        //usleep(100);

        //read data
        ret=read(fp,mem,count);
        if(ret!=1)
        {
                //printf("(error read)%x:%x\n",reg,ret);
                return -2;
        }
}
int writei2c(int device,unsigned char reg,unsigned char* mem,int count)
{
        //write address
        int ret=write(fp,&reg,1);
        if(ret!=1)
        {
                //printf("(error write)%x:%x\n",reg,ret);
                return -1;
        }
        //usleep(100);

        //read data
        ret=write(fp,mem,count);
        if(ret!=1)
        {
                //printf("(error write)%x:%x\n",reg,ret);
                return -2;
        }
}
*/
void main()
{
        int ret;
        unsigned char buf[18];

        //open
        fp = open("/dev/i2c-1",O_RDWR);
        if(fp<0)
        {
                printf("error open\n");
                return;
        }

        while(1)
        {
                //device,register,memory,count
                systemi2c_read(0x68,0x3b,buf+0,14);

                printf("(%.5d,%.5d,%.5d),       ",
                        (signed short)((buf[0]<<8)|buf[1]),
                        (signed short)((buf[2]<<8)|buf[3]),
                        (signed short)((buf[4]<<8)|buf[5])
                );
                printf("(%.5d,%.5d,%.5d)\n",
                        (signed short)((buf[8]<<8)|buf[9]),
                        (signed short)((buf[10]<<8)|buf[11]),
                        (signed short)((buf[12]<<8)|buf[13])
                );
        }
}

                                                                          137,0-1       Bot

