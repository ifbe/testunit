#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>

//
int fp;
unsigned char outbuf[16];

//
static unsigned char reg[12];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
float measure[6];




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




int initlsm303d()
{
	//LSM303_CTRL1
	reg[0]=0xc7;
	systemi2c_write(0x1d,0x20,reg,1);
	usleep(1000);

	//LSM303_CTRL2
	reg[0]=0x08;
	systemi2c_write(0x1d,0x21,reg,1);
	usleep(1000);

	//LSM303_CTRL5
	reg[0]=0x64;
	systemi2c_write(0x1d,0x24,reg,1);
	usleep(1000);

	//LSM303_CTRL6
	reg[0]=0x20;
	systemi2c_write(0x1d,0x25,reg,1);
	usleep(1000);

	//LSM303_CTRL7
	reg[0]=0x0;
	systemi2c_write(0x1d,0x26,reg,1);
	usleep(1000);




	//
	return 1;
}
void killlsm303d()
{
}




int readlsm303d()
{
	int temp;

	//accel
	systemi2c_read(0x1d, 0x28, reg, 1);
	systemi2c_read(0x1d, 0x29, reg+1, 1);
	systemi2c_read(0x1d, 0x2a, reg+2, 1);
	systemi2c_read(0x1d, 0x2b, reg+3, 1);
	systemi2c_read(0x1d, 0x2c, reg+4, 1);
	systemi2c_read(0x1d, 0x2d, reg+5, 1);

	//mag
	systemi2c_read(0x1d, 0x8, reg+6, 1);
	systemi2c_read(0x1d, 0x9, reg+7, 1);
	systemi2c_read(0x1d, 0xa, reg+8, 1);
	systemi2c_read(0x1d, 0xb, reg+9, 1);
	systemi2c_read(0x1d, 0xc, reg+10, 1);
	systemi2c_read(0x1d, 0xd, reg+11, 1);


	for(temp=0;temp<12;temp++)
	{
		printf("%.2x ",reg[temp]);
	}
	printf("\n");






	//
	temp = *(short*)(reg+0x0);
	measure[0] = temp *2* 9.8 / 16384;

	temp=*(short*)(reg+0x2);
	measure[1] = temp *2* 9.8 / 16384;

	temp=*(short*)(reg+0x4);
	measure[2] = temp *2* 9.8 / 16384;

	//
	temp=*(short*)(reg+0x6);
	measure[3] = temp;

	temp=*(short*)(reg+0x8);
	measure[4] = temp;

	temp=*(short*)(reg+10);
	measure[5] = temp;




	printf("%1.3f	%1.3f	%1.3f	%1.3f	%1.3f	%1.3f\n",
		measure[0],
		measure[1],
		measure[2],
		measure[3],
		measure[4],
		measure[5]
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
	initlsm303d();

	while(1)
	{
		readlsm303d();
	}
}
