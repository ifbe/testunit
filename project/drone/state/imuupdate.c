#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>
#define Kp 100.0f
#define Ki 0.005f

//
extern float smooth[20];
extern int timeinterval;

//????
static float integralx;
static float integraly;
static float integralz;

//quaternion
static float q0;
static float q1;
static float q2;
static float q3;

//eulerian
float imu[3];




int initimuupdate()
{
	q0=1.0;
	q1=q2=q3=0;

	integralx=integraly=integralz=0;

	return 1;
}
void killimuupdate()
{
}




//accel + gyro
void imuupdate()
{
	float vx, vy, vz;
	float ex, ey, ez;
	float norm,T;

	//value
	float ax=smooth[10];
	float ay=smooth[11];
	float az=smooth[12];
	float gx=smooth[13];
	float gy=smooth[14];
	float gz=smooth[15];
/*
	printf("%f	%f	%f\n",
		gx,gy,gz
	);
*/
	//time
	T=(float)timeinterval / 1000.0 / 1000.0;
	//printf("T=%f\n",T);

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

	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*T;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*T;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*T;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*T;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
/*
	printf("q:	%f	%f	%f	%f\n",
		q0,q1,q2,q3
	);
*/
	imu[0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	imu[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	imu[2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;
/*
	printf("imu:	%f	%f	%f\n",
		imu[0],
		imu[1],
		imu[2]
	);
*/
}
