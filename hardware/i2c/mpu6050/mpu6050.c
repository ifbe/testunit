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

//mpu6050's plaything
unsigned char reg[0x20];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
float measuredata[10];




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




int initmpu6050()
{
	//PWR_MGMT_1	reset
	reg[0]=0x80;
	systemi2c_write(0x68,0x6b,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//PWR_MGMT_1	clock
	reg[0]=0x01;
	systemi2c_write(0x68,0x6b,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//PWR_MGMT_2	enable
	reg[0]=0x00;
	systemi2c_write(0x68,0x6c,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//CONFIG
	reg[0]=0x3;
	systemi2c_write(0x68,0x1a,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//SMPLRT_DIV
	reg[0]=0x4;
	systemi2c_write(0x68,0x19,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//GYRO_CONFIG
	systemi2c_read(0x68,0x1b,reg,1);
	reg[0] &= 0xe5;
	reg[0] |= (3<<3);
	systemi2c_write(0x68,0x1b,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//ACCEL_CONFIG
	systemi2c_read(0x68,0x1c,reg,1);
	reg[0] &= 0xe7;
	reg[0] |= (1<<3);
	systemi2c_write(0x68,0x1c,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//ACCEL_CONFIG2
	systemi2c_read(0x68,0x1d,reg,1);
	reg[0] &= 0xf0;
	reg[0] |= 0x3;
	systemi2c_write(0x68,0x1d,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//INT_PIN_CFG
	reg[0]=0x22;
	systemi2c_write(0x68,0x37,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);

	//INT_ENABLE
	reg[0]=0x1;
	systemi2c_write(0x68,0x38,reg,1);
//system("i2cdump -y 1 0x68");
usleep(1000);




	//
	return 1;
}
void killmpu6050()
{
}




int mpu6050()
{
	int temp;

	//0x68.0x3b -> 0x00
	systemi2c_read(0x68, 0x3b, reg+0, 14);




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	measuredata[0] = temp * 9.8 / 4096.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	measuredata[1] = temp * 9.8 / 4096.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	measuredata[2] = temp * 9.8 / 4096.0;




	//temp
	temp=(reg[6]<<8) + reg[7];
	measuredata[9] = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	measuredata[3] = temp / 57.3 / 16.4;

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	measuredata[4] = temp / 57.3 / 16.4;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	measuredata[5] = temp / 57.3 / 16.4;





	printf("%1.3f	%1.3f	%1.3f	%1.3f	%1.3f	%1.3f\n",
		measuredata[0],
		measuredata[1],
		measuredata[2],
		measuredata[3],
		measuredata[4],
		measuredata[5]
	);

	//printf("6050:	%f\n",measuredata[9]);
}




void main()
{
        fp = open("/dev/i2c-1",O_RDWR);
        if(fp<0)
        {
                printf("error open\n");
                return;
        }
	initmpu6050();

	while(1)
	{
		mpu6050();
	}
	killmpu6050();
}
