#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define u8 unsigned char




int fp;
int systemi2c_init()
{
	fp = open("/dev/i2c-1",O_RDWR);
	if(fp<0)
	{
		printf("error open\n");
		return fp;
	}
}
int systemi2c_free()
{
	return 0;
}
int systemi2c_write(u8 dev, u8 reg, u8* buf, u8 len)
{
	int ret;
	unsigned char outbuf[32];
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




void initpca9685()
{
	unsigned char buf[16];
	int ret;

	//hardwarepwm(pca9685)
	buf[0]=0x31;	//sleep
	systemi2c_write(0x40, 0, buf, 1);
	usleep(1000);

	buf[0]=132;	//prescale
	systemi2c_write(0x40, 0xfe, buf, 1);
	usleep(1000);

	buf[0]=0xa1;	//wake
	systemi2c_write(0x40, 0, buf, 1);
	usleep(1000);

	buf[0]=0x4;	//restart
	systemi2c_write(0x40, 1, buf, 1);
	usleep(1000);

	buf[0]=0;		//T=3ms
	buf[1]=0;
	buf[2]=3000 & 0xff;
	buf[3]=3000 >> 8;

	buf[4]=0;		//T=3ms
	buf[5]=0;
	buf[6]=buf[2];
	buf[7]=buf[3];

	buf[8]=0;		//T=3ms
	buf[9]=0;
	buf[10]=buf[2];
	buf[11]=buf[3];

	buf[12]=0;		//T=3ms
	buf[13]=0;
	buf[14]=buf[2];
	buf[15]=buf[3];

	systemi2c_write(0x40, 6, buf, 16);
	usleep(1000);
/*
	int x,y;
	for(y=0;y<16;y++)
	{
		for(x=0;x<16;x++)
		{
			systemi2c_read(0x40, y*16+x, buf+x, 1);
	usleep(1000);
			printf("%.2x ",buf[x]);
		}
		printf("\n");
	}
*/
}

void main()
{
	int ret;
	unsigned char buf[24];

	systemi2c_init();
	initpca9685();

	while(1)
	{
		scanf("%d",&ret);

		buf[0] = 0;
		buf[1] = 0;
		buf[2] = (0+ret) & 0xff;
		buf[3] = (0+ret) >> 8;

		buf[4] = 0;
		buf[5] = 0;
		buf[6] = (0+ret) & 0xff;
		buf[7] = (0+ret) >> 8;

		buf[8] = 0;
		buf[9] = 0;
		buf[10] = (0+ret) & 0xff;
		buf[11] = (0+ret) >> 8;

		buf[12] = 0;
		buf[13] = 0;
		buf[14] = (0+ret) & 0xff;
		buf[15] = (0+ret) >> 8;

		buf[16] = 0;
		buf[17] = 0;
		buf[18] = (0+ret) & 0xff;
		buf[19] = (0+ret) >> 8;

		buf[20] = 0;
		buf[21] = 0;
		buf[22] = (0+ret) & 0xff;
		buf[23] = (0+ret) >> 8;

		systemi2c_write(0x40,0x6,buf+2,24);
	}
}
