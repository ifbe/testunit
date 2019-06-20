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
#define Kp 100.0
#define Ki 0.005
#define halfT 0.001




int fp;
float measure[10];

//????
static float integralx = 0.0;
static float integraly = 0.0;
static float integralz = 0.0;

//quaternion
static float q0 = 1.0;
static float q1 = 0.0;
static float q2 = 0.0;
static float q3 = 0.0;

//
float eulerian[3];




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




int initmpu6050()
{
	unsigned char reg[0x20];

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
	reg[0] |= (0<<3);
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




int readmpu6050()
{
	int temp;
	unsigned char reg[0x20];

	//0x68.0x3b -> 0x00
	systemi2c_read(0x68, 0x3b, reg+0, 14);




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	measure[0] = temp * 9.8 / 8192.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	measure[1] = temp * 9.8 / 8192.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	measure[2] = temp * 9.8 / 8192.0;




	//temp
	temp=(reg[6]<<8) + reg[7];
	measure[9] = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	measure[3] = temp / 57.3 / 16.4;

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	measure[4] = temp / 57.3 / 16.4;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	measure[5] = temp / 57.3 / 16.4;





	printf("raw:	%1.3f	%1.3f	%1.3f	%1.3f	%1.3f	%1.3f\n",
		measure[0],
		measure[1],
		measure[2],
		measure[3],
		measure[4],
		measure[5]
	);

	//printf("6050:	%f\n",measure[9]);
}




void imuupdate()
{
	float vx, vy, vz;
	float ex, ey, ez;
	float norm;

	//value
	float ax = measure[0];
	float ay = measure[1];
	float az = measure[2];

	float gx = measure[3];
	float gy = measure[4];
	float gz = measure[5];

	//a
	norm = sqrt(ax*ax+ay*ay+az*az);
	ax = ax/norm;
	ay = ay/norm;
	az = az/norm;

	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	integralx += ex*Ki;
	integraly += ey*Ki;
	integralz += ez*Ki;

	gx = gx + Kp*ex + integralx;
	gy = gy + Kp*ey + integraly;
	gz = gz + Kp*ez + integralz;

	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
	printf("qua:	%f	%f	%f	%f\n", q0, q1, q2, q3);

	eulerian[0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	eulerian[2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;

	printf("imu:	%lf	%lf	%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
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

	initmpu6050();

	while(1)
	{
		readmpu6050();
		imuupdate();
	}
}
