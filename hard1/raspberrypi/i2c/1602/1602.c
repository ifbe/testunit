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




int main(int argc, char** argv)
{
	return 0;
}
