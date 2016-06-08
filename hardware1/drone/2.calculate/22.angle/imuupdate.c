#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>
#define Kp 2.0f
#define Ki 0.005f

//input
extern float smooth[20];
extern int timeinterval;

//inner
static float integral[6];
static float q[8];

//output
extern float eulerian[3];




int initimuupdate()
{
	q[0]=1.0;
	q[1]=q[2]=q[3]=0;
	q[4]=1.0;
	q[5]=q[6]=q[7]=0;

	integral[0]=integral[1]=integral[2]=integral[3]=integral[4]=integral[5]=0;

	return 1;
}
void killimuupdate()
{
}




//accel + gyro
void imuupdate(int N)
{
	float norm,T;
	float vx, vy, vz;
	float ex, ey, ez;

	//pop
	float q0=q[4*N + 0];
	float q1=q[4*N + 1];
	float q2=q[4*N + 2];
	float q3=q[4*N + 3];

	//value
	float ax=smooth[10*N + 0];
	float ay=smooth[10*N + 1];
	float az=smooth[10*N + 2];
	float gx=smooth[10*N + 3];
	float gy=smooth[10*N + 4];
	float gz=smooth[10*N + 5];
/*
	printf("%f	%f	%f\n", ax,ay,az );
	printf("%f	%f	%f\n", gx,gy,gz );
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

	integral[3*N + 0] += ex*Ki;
	integral[3*N + 1] += ey*Ki;
	integral[3*N + 2] += ez*Ki;

	gx = gx + Kp*ex + integral[3*N + 0];
	gy = gy + Kp*ey + integral[3*N + 1];
	gz = gz + Kp*ez + integral[3*N + 2];

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
	eulerian[3*N + 0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	eulerian[3*N + 1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	eulerian[3*N + 2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;

	//push
	q[4*N + 0]=q0;
	q[4*N + 1]=q1;
	q[4*N + 2]=q2;
	q[4*N + 3]=q3;
}
