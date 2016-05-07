#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>

#define Kp 2.0f
#define Ki 0.002f
#define halfT 0.001f

extern float predictdata[6];
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
float exInt = 0, eyInt = 0, ezInt = 0;
float eulerian[3];

int initquaternion()
{
	q0=1;
	q1=q2=q3=0;
	exInt=eyInt=ezInt=0;

	return 1;
}
void imuupdate()
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;

	float ax=predictdata[0]*9.8/16384;
	float ay=predictdata[1]*9.8/16384;
	float az=predictdata[2]*9.8/16384;
	float gx=predictdata[3]*3.141592653/16384;
	float gy=predictdata[4]*3.141592653/16384;
	float gz=predictdata[5]*3.141592653/16384;

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

	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	eulerian[0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*57.3;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*57.3;
	eulerian[2] = asin(2*q0*q2 - 2*q1*q3)*57.3;

	printf("%lf,%lf,%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);

}
