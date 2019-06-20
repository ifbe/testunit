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
#define Kp 100.0f
#define Ki 0.005f




//ak8963
float ak8963_measure[3];
short xmin=-139;
short xmax=243;
short ymin=87;
short ymax=472;
short zmin=-146;
short zmax=294;
//mpu9250
float mpu9250_measure[10];
//i
float integralx = 0.0;
float integraly = 0.0;
float integralz = 0.0;
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




int initmpu9250()
{
	u8 reg[0x20];

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

	return 1;
}
int freempu9250()
{
	return 0;
}
int readmpu9250()
{
	int temp;
	u8 reg[0x20];

	//0x68.0x3b -> 0x00
	systemi2c_read(0x68, 0x3b, reg+0, 14);




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[0] = temp * 9.8 / 16384.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[1] = temp * 9.8 / 16384.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[2] = temp * 9.8 / 16384.0;




	//temp
	temp=(reg[6]<<8) + reg[7];
	mpu9250_measure[9] = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[3] = temp / 57.3 / 16.4;

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[4] = temp / 57.3 / 16.4;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	mpu9250_measure[5] = temp / 57.3 / 16.4;





	printf("9250:	%f	%f	%f	%f	%f	%f\n",
		mpu9250_measure[0],
		mpu9250_measure[1],
		mpu9250_measure[2],
		mpu9250_measure[3],
		mpu9250_measure[4],
		mpu9250_measure[5]
	);

	//printf("9250:	%f\n",mpu9250_measure[9]);
}



void mahonyahrsupdate()
{
	float norm,T;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	//value
	float ax = mpu9250_measure[0];
	float ay = mpu9250_measure[1];
	float az = mpu9250_measure[2];

	float gx = mpu9250_measure[0];
	float gy = mpu9250_measure[1];
	float gz = mpu9250_measure[2];

	float mx = ak8963_measure[0];
	float my = ak8963_measure[1];
	float mz = ak8963_measure[2];

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
	hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
	hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));

	bx = sqrt(hx * hx + hy * hy);
	bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));


	// Estimated direction of gravity and magnetic field
	halfvx = q1q3 - q0q2;
	halfvy = q0q1 + q2q3;
	halfvz = q0q0 - 0.5f + q3q3;

	halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
	halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
	halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);  

	// Error is sum of cross product between estimated direction and measured direction of field vectors
	halfex = (ay * halfvz - az * halfvy) + (my * halfwz - mz * halfwy);
	halfey = (az * halfvx - ax * halfvz) + (mz * halfwx - mx * halfwz);
	halfez = (ax * halfvy - ay * halfvx) + (mx * halfwy - my * halfwx);

	// Compute and apply integral feedback if enabled
	if(Ki > 0.0f)
	{
		// integral error scaled by Ki
		integralx += Ki * halfex * T;
		integraly += Ki * halfey * T;
		integralz += Ki * halfez * T;

		gx += integralx;	// apply integral feedback
		gy += integraly;
		gz += integralz;
	}
	else
	{
		integralx = 0.0f;	// prevent integral windup
		integraly = 0.0f;
		integralz = 0.0f;
	}

	// Apply proportional feedback
	gx += Kp * halfex;
	gy += Kp * halfey;
	gz += Kp * halfez;
	

	// Integrate rate of change of quaternion
	gx *= 0.5f * T;		// pre-multiply common factors
	gy *= 0.5f * T;
	gz *= 0.5f * T;
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx); 
	

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
	initmpu9250();
	initak8963();

	while(1)
	{
		readmpu9250();
		readak8963();
		mahonyahrsupdate();
	}
	freempu9250();
}
