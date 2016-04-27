#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>
#define BYTE unsigned char
int fp;

int pcf8591_read(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
	int i;
	int ret;
	unsigned char thisreg;

	//select device
	ret=ioctl(fp,I2C_SLAVE,dev);
	if(ret < 0)
	{
		printf("error@systemi2c_read.ioctl\n");
		return;
	}

	for(i=0;i<count;i++)
	{
		//select register
		thisreg=reg+i;
		ret=write(fp,&thisreg,1);
		if(ret!=1)
		{
			printf("error@systemi2c_read.writereg:%x\n",ret);
			return -1;
		}

		//read data
		ret=read(fp,buf+i,1);
		ret=read(fp,buf+i,1);
		if(ret<=0)
		{
			printf("error@systemi2c_read.readdata:%x\n",ret);
			return -2;
		}
	}
	return 1;
}
void main()
{
	int ret;
	char buffer[4];

	//open
	fp = open("/dev/i2c-1",O_RDWR);
	if(fp<0)
	{
		printf("error open\n");
		return;
	}

	while(1)
	{
		pcf8591_read(0x48,0x42,buffer,4);
		printf("(%d,%d),(%d,%d))\n",
			buffer[0]-0x80,
			buffer[1]-0x80,
			0x80-buffer[2],
			0x80-buffer[3]
		);
	}
}

