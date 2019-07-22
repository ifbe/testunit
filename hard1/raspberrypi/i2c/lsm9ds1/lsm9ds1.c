#include <math.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define PI 3.1415926535897932384626433832795
#define u8 unsigned char

//
#define Kp 100
#define Ki 0.005
#define halfT 0.0025

//0=250dps, 1=500dps, 2=1000dps, 3=2000dps
#define gyr_cfg 2
#define gyr_max 1000

//0=2g, 1=4g, 2=8g, 3=16g
#define acc_cfg 0
#define acc_max 2*9.8




float measure[10];
//i
float exInt = 0.0;
float eyInt = 0.0;
float ezInt = 0.0;
//quaternion
float q0 = 1.0;
float q1 = 0.0;
float q2 = 0.0;
float q3 = 0.0;
//eulerian
float eulerian[3];




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




int initlsm9ds1()
{
	return 1;
}
int freelsm9ds1()
{
	return 0;
}
int readlsm9ds1()
{
	short t;
	u8 reg[32];

	t = systemi2c_read(0x6a, 0x18, reg+0, 6);
	t = systemi2c_read(0x6a, 0x19, reg+1, 5);
	t = systemi2c_read(0x6a, 0x1a, reg+2, 4);
	t = systemi2c_read(0x6a, 0x1b, reg+3, 3);
	t = systemi2c_read(0x6a, 0x1c, reg+4, 2);
	t = systemi2c_read(0x6a, 0x1d, reg+5, 1);
	//printf("%02x %02x %02x %02x %02x %02x\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5]);

	t = *(short*)(reg+0);
	measure[0] = t * 500.0/32768 * PI/180.0;
	t = *(short*)(reg+2);
	measure[1] =-t * 500.0/32768 * PI/180.0;
	t = *(short*)(reg+4);
	measure[2] = t * 500.0/32768 * PI/180.0;


	systemi2c_read(0x6a, 0x28, reg+0, 6);
	systemi2c_read(0x6a, 0x29, reg+1, 5);
	systemi2c_read(0x6a, 0x2a, reg+2, 4);
	systemi2c_read(0x6a, 0x2b, reg+3, 3);
	systemi2c_read(0x6a, 0x2c, reg+4, 2);
	systemi2c_read(0x6a, 0x2d, reg+5, 1);
	//printf("%02x %02x %02x %02x %02x %02x\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5]);

	t = *(short*)(reg+0);
	measure[3] = t * 9.8*8 / 32768.0;
	t = *(short*)(reg+2);
	measure[4] =-t * 9.8*8 / 32768.0;
	t = *(short*)(reg+4);
	measure[5] = t * 9.8*8 / 32768.0;


	systemi2c_read(0x1c, 0x28, reg+0, 6);
	systemi2c_read(0x1c, 0x29, reg+1, 5);
	systemi2c_read(0x1c, 0x2a, reg+2, 4);
	systemi2c_read(0x1c, 0x2b, reg+3, 3);
	systemi2c_read(0x1c, 0x2c, reg+4, 2);
	systemi2c_read(0x1c, 0x2d, reg+5, 1);
	//printf("%02x %02x %02x %02x %02x %02x\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5]);

	t = *(short*)(reg+0);
	measure[6] =-t * 400.0 / 32768.0;
	t = *(short*)(reg+2);
	measure[7] =-t * 400.0 / 32768.0;
	t = *(short*)(reg+4);
	measure[8] = t * 400.0 / 32768.0;
}



void mahonyahrsupdate()
{
	float norm,T;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
	float hx, hy, bx, bz;
	float vx, vy, vz, wx, wy, wz, ex, ey, ez;

	//value
	float gx = measure[0];
	float gy = measure[1];
	float gz = measure[2];

	float ax = measure[3];
	float ay = measure[4];
	float az = measure[5];

	float mx = measure[6];
	float my = measure[7];
	float mz = measure[8];

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	ax /= norm;
	ay /= norm;
	az /= norm;     

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	mx /= norm;
	my /= norm;
	mz /= norm;   

	// Auxiliary variables to avoid repeated arithmetic
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;   

	// Reference direction of Earth's magnetic field
	hx = 2.0*mx*(0.5 - q2q2 - q3q3) + 2.0*my*(q1q2 - q0q3) + 2.0*mz*(q1q3 + q0q2);
	hy = 2.0*mx*(q1q2 + q0q3) + 2.0*my*(0.5 - q1q1 - q3q3) + 2.0*mz*(q2q3 - q0q1);

	bx = sqrt(hx * hx + hy * hy);
	bz = 2.0*mx*(q1q3 - q0q2) + 2.0*my*(q2q3 + q0q1) + 2.0*mz*(0.5 - q1q1 - q2q2);

	// Estimated direction of gravity
	vx = 2.0*(q1q3 - q0q2);
	vy = 2.0*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;

	// Estimated direction of magnetic
	wx = 2.0*bx*(0.5 - q2q2 - q3q3) + 2.0*bz*(q1q3 - q0q2);
	wy = 2.0*bx*(q1q2 - q0q3) + 2.0*bz*(q0q1 + q2q3);
	wz = 2.0*bx*(q0q2 + q1q3) + 2.0*bz*(0.5 - q1q1 - q2q2);  

	// Error is sum of cross product between estimated direction and measured direction of field vectors
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);

	// Compute and apply integral feedback if enabled
	exInt = exInt + ex * Ki * halfT;
	eyInt = eyInt + ey * Ki * halfT;
	ezInt = ezInt + ez * Ki * halfT;

	// Apply proportional feedback
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	// Integrate rate of change of quaternion
	q0 += (-q1 * gx - q2 * gy - q3 * gz)*halfT;
	q1 += (q0 * gx + q2 * gz - q3 * gy)*halfT;
	q2 += (q0 * gy - q1 * gz + q3 * gx)*halfT;
	q3 += (q0 * gz + q1 * gy - q2 * gx)*halfT;
	

	// Normalise quaternion
	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 /= norm;
	q1 /= norm;
	q2 /= norm;
	q3 /= norm;

	eulerian[0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	eulerian[2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;

	printf("ahrs:	%lf	%lf	%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);

}




void main()
{
	systemi2c_init();
	initlsm9ds1();

	while(1)
	{
		readlsm9ds1();
		//printf(	"%f	%f	%f	%f	%f	%f	%f	%f	%f\n",
			//measure[0],measure[1],measure[2],measure[3],measure[4],measure[5],measure[6],measure[7],measure[8]);
		mahonyahrsupdate();
	}
}
