#include <math.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define u8 unsigned char




float ak8963_measure[3];
short xmin=-139;
short xmax=243;
short ymin=87;
short ymax=472;
short zmin=-146;
short zmax=294;




int fp;
int systemi2c_init()
{
	fp = open("/dev/i2c-1",O_RDWR);
	if(fp<0)
	{
		printf("error open\n");
		return 0;
	}
}
int systemi2c_free()
{
	return 0;
}
int systemi2c_write(u8 dev, u8 reg, u8* buf, u8 len)
{
	int ret;
	unsigned char outbuf[16];
	struct i2c_msg messages[1];
	struct i2c_rdwr_ioctl_data packets;

	//which,what0,what1,what2......
	outbuf[0] = reg;
	for(ret=0;ret<len;ret++)
	{
		outbuf[ret+1] = buf[ret];
	}

	//message
	messages[0].addr  = dev;
	messages[0].flags = 0;
	messages[0].len   = len+1;
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

int systemi2c_read(u8 dev, u8 reg, u8* buf, u8 len)
{
	unsigned char outbuf[16];
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
	messages[1].len   = len;
	messages[1].buf   = buf;

	//send
	packets.msgs      = messages;
	packets.nmsgs     = 2;
	if(ioctl(fp, I2C_RDWR, &packets) < 0)
	{
		perror("Unable to send data");
		return -1;
	}

	return 1;
}




int initak8963()
{
	u8 buf[0x10];

	//AK8963_CNT2
	buf[0]=0x1;
	systemi2c_write(0xc,0xb,buf,1);
	usleep(1000);

	//AK8963_CNT2
	buf[0]=0x16;
	systemi2c_write(0xc,0xa,buf,1);
	usleep(1000);

	//
	return 1;
}
int freeak8963()
{
	return 0;
}
int readak8963()
{
	int temp;
	u8 buf[0x10];

	//0xc.0x3b -> 0x20
	systemi2c_read(0xc, 3, buf+0x3, 7);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",buf[0x3+temp]);
	}
	printf("\n");
*/
	//mag
	temp = *(short*)(buf+0x3);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (xmin+xmax)/2;
	ak8963_measure[0] = temp * 4912.0 / 32760.0;

	temp=*(short*)(buf+0x5);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (ymin+ymax)/2;
	ak8963_measure[1] = temp * 4912.0 / 32760.0;

	temp=*(short*)(buf+0x7)	+1;	//ensure not 0
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (zmin+zmax)/2;
	ak8963_measure[2] = temp * 4912.0 / 32760.0;
/*
	printf("%d	%d	%d	%d	%d	%d\n",
		xmin,
		xmax,
		ymin,
		ymax,
		zmin,
		zmax
	);
*/
	printf("8963:	%f	%f	%f\n",
		ak8963_measure[0],
		ak8963_measure[1],
		ak8963_measure[2]
	);

}




void main()
{
	systemi2c_init();
	initak8963();

	while(1)
	{
		readak8963();
	}
}
