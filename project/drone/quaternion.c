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
extern int timeinterval;
extern float predictdata[9];

//????
static float exInt;
static float eyInt;
static float ezInt;

//raw
static float ax;
static float ay;
static float az;
static float gx;
static float gy;
static float gz;
static float mx;
static float my;
static float mz;

//quaternion
static float q0;
static float q1;
static float q2;
static float q3;

//eulerian
float eulerian[3];




int initquaternion()
{
	q0=1.0;
	q1=q2=q3=0;

	exInt=eyInt=ezInt=0;

	return 1;
}
void killquaternion()
{
}




void imuupdate()
{
	float vx, vy, vz;
	float ex, ey, ez;
	float norm,halfT;

	//time
	halfT=(float)timeinterval / 1000.0 / 1000.0 / 2.0;

	//value
	float ax=predictdata[0];
	float ay=predictdata[1];
	float az=predictdata[2];
	float gx=predictdata[3];
	float gy=predictdata[4];
	float gz=predictdata[5];

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

	eulerian[0] = asin(2*q0*q2 - 2*q1*q3)*57.3;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*57.3;
	eulerian[2] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*57.3;

	printf("imu:	%lf,%lf,%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);

}
void ahrsupdate()
{
	float bx, bz;
	float hx, hy, hz;
	float vx, vy, vz;
	float wx, wy, wz;
	float ex, ey, ez;
	float tempq0,tempq1,tempq2,tempq3;
	float norm,halfT;

	// auxiliary variables to reduce number of repeated operations
	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;

	//time
	halfT=(float)timeinterval / 1000.0 / 1000.0 / 2.0;
	//printf("halfT=%f\n",halfT);

	//value
	ax=predictdata[0];
	ay=predictdata[1];
	az=predictdata[2];
	gx=predictdata[3];
	gy=predictdata[4];
	gz=predictdata[5];
	mx=predictdata[6];
	my=predictdata[7];
	mz=predictdata[8];

	//a
	norm = sqrt(ax*ax + ay*ay + az*az);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;
	//printf("a:	%f,%f,%f\n",ax,ay,az);

	//m
	norm = sqrt(mx*mx + my*my + mz*mz);
	mx = mx / norm;
	my = my / norm;
	mz = mz / norm;
	//printf("m:	%f,%f,%f\n",mx,my,mz);

	//compute reference direction of flux
	hx = 2*mx*(0.5 - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
	hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5 - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
	hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5 - q1q1 - q2q2);
	printf("h:	%f,%f,%f\n",hx,hy,hz);

	//b
	bx = sqrt((hx*hx) + (hy*hy));
	bz = hz;
	//printf("b:	%f,%f\n",bx,bz);

	// estimated direction of gravity and flux (v and w)
	vx = 2*(q1q3 - q0q2);
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	//printf("v:	%f,%f,%f\n",vx,vy,vz);

	//
	wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
	wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
	wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);
	//printf("w:	%f,%f,%f\n",wx,wy,wz);

	//sum of cross product between reference direction and sensor direction
	ex = (ay*vz - az*vy) + (my*wz - mz*wy);
	ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
	ez = (ax*vy - ay*vx) + (mx*wy - my*wx);
	//printf("e:	%f,%f,%f\n",ex,ey,ez);

	// integral error scaled integral gain
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;
	//printf("e?Int:	%f,%f,%f\n",exInt,eyInt,ezInt);

	// adjusted gyroscope measurements
	//printf("g:	%f,%f,%f\n",gx,gy,gz);
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
	//printf("g:	%f,%f,%f\n",gx,gy,gz);

	// integrate quaternion rate and normalise
	tempq0 = (-q1*gx - q2*gy - q3*gz)*halfT;
	tempq1 = (q0*gx + q2*gz - q3*gy)*halfT;
	tempq2 = (q0*gy - q1*gz + q3*gx)*halfT;
	tempq3 = (q0*gz + q1*gy - q2*gx)*halfT;
	q0=tempq0;
	q1=tempq1;
	q2=tempq2;
	q3=tempq3;
	//printf("q:	%f,%f,%f,%f\n", q0, q1, q2, q3);

	// normalise quaternion
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
	//printf("q:	%f,%f,%f,%f\n",q0,q1,q2,q3);

	eulerian[0] = asin(2*q0*q2 - 2*q1*q3)*57.3;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*57.3;
	eulerian[2] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*57.3;
/*
	printf("ahrs:	%3.3lf,	%3.3lf,	%3.3lf,	%3.3lf,	%3.3lf\n",
		2*q0*q2 - 2*q1*q3,
		2*(q0*q3+q1*q2), 1-2*(q2*q2+q3*q3),
		2*(q0*q1+q2*q3), 1-2*(q1*q1+q2*q2)
	);
*/
/*
	printf("ahrs:	%lf,%lf,%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);
*/
}

