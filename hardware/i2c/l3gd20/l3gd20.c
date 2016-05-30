#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>




int fp;
unsigned char outbuf[16];

//l3gd20's plaything
static unsigned char reg[0x10];

//gx,gy,gz
float measure[3];




int systemi2c_write(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
	int ret;
	struct i2c_msg messages[1];
	struct i2c_rdwr_ioctl_data packets;

	//which,what0,what1,what2......
	outbuf[0] = reg;
	for(ret=0;ret<count;ret++)
	{
		outbuf[ret+1] = buf[ret];
	}

	//message
	messages[0].addr  = dev;
	messages[0].flags = 0;
	messages[0].len   = count+1;
	messages[0].buf   = outbuf;

	//transfer
	packets.msgs  = messages;
	packets.nmsgs = 1;
	ret=ioctl(fp, I2C_RDWR, &packets);
	if(ret<0)
	{
		perror("Unable to send data");
		return -1;
	}

	return 1;
}

int systemi2c_read(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
	struct i2c_msg messages[2];
	struct i2c_rdwr_ioctl_data packets;

	//out
	outbuf[0] = reg;
	messages[0].addr  = dev;
	messages[0].flags = 0;
	messages[0].len   = 1;
	messages[0].buf   = outbuf;

	//in
	messages[1].addr  = dev;
	messages[1].flags = I2C_M_RD;
	messages[1].len   = count;
	messages[1].buf   = buf;

	//send
	packets.msgs      = messages;
	packets.nmsgs     = 2;
	if(ioctl(fp, I2C_RDWR, &packets) < 0)
	{
		perror("Unable to send data");
		return 1;
	}

	return 1;
}



int initl3gd20()
{
	//CTRL_REG1
	reg[0]=0xf;
	systemi2c_write(0x6b,0x20,reg,1);
	usleep(1000);

	//CTRL_REG4
	reg[0]=0x20;	//2000dps
	systemi2c_write(0x6b,0x23,reg,1);
	usleep(1000);




	//
	return 1;
}
void killl3gd20()
{
}




int readl3gd20()
{
	int temp;

	//0xc.0x3b -> 0x20
	systemi2c_read(0x6b, 0x28, reg, 1);
	systemi2c_read(0x6b, 0x29, reg+1, 1);
	systemi2c_read(0x6b, 0x2a, reg+2, 1);
	systemi2c_read(0x6b, 0x2b, reg+3, 1);
	systemi2c_read(0x6b, 0x2c, reg+4, 1);
	systemi2c_read(0x6b, 0x2d, reg+5, 1);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/





	//mag
	temp = *(short*)(reg+0x0);
	measure[0] = temp / 16.4 / 57.3;	//2000dps?

	temp=*(short*)(reg+0x2);
	measure[1] = temp / 16.4 / 57.3;

	temp=*(short*)(reg+0x4);
	measure[2] = temp / 16.4 / 57.3;




	printf("l3gd20:	%1.3f	%1.3f	%1.3f\n",
		measure[0],
		measure[1],
		measure[2]
	);
}


void main()
{
	fp = open("/dev/i2c-1",O_RDWR);
	if(fp<0)
	{
		printf("error open\n");
		return;
	}

	initl3gd20();

	while(1)
	{
		readl3gd20();
	}
}
