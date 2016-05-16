#define BYTE unsigned char
#include<errno.h>
#include<stdio.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>

int fp;
unsigned char outbuf[16];
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
		//perror("Unable to send data");
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
		//perror("Unable to send data");
		return 1;
	}

	return 1;
}

void initpca9685()
{
	unsigned char buf[16];

	//sleep
	buf[0]=0x31;
	systemi2c_write(0x40, 0, buf, 1);

	//prescale
	buf[0]=0x20;
	systemi2c_write(0x40, 0xef, buf, 1);

	//wake
	buf[0]=0xa1;
	systemi2c_write(0x40, 0, buf, 1);

	//restart
	buf[0]=0x4;
	systemi2c_write(0x40, 1, buf, 1);

	//default value
	buf[0]=0;
	buf[1]=0;
	buf[2]=3000&0xff;
	buf[3]=3000>>8;
	systemi2c_write(0x40, 6, buf, 4);

	int x,y;
	for(y=0;y<16;y++)
	{
		for(x=0;x<16;x++)
		{
			systemi2c_read(0x40, y*16+x, buf+x, 1);
			printf("%.2x ",buf[x]);
		}
		printf("\n");
	}
}

void main()
{
	unsigned char buf[18];

	//open
	fp = open("/dev/i2c-1",O_RDWR);
	if(fp<0)
	{
		printf("error open\n");
		return;
	}
	initpca9685();

	while(1)
	{
		scanf("%d",(int*)buf);
		printf("%d,%d\n",buf[0],buf[1]);
		systemi2c_write(0x40,0x8,buf+0,2);
	}
}

