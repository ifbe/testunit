#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>
#define u64 unsigned long long
#define Kp 2.0f
#define Ki 0.005f

//input
extern u64 timeinterval;
extern float smooth[20];

//memory
static float integralx;
static float integraly;
static float integralz;
static float q0;
static float q1;
static float q2;
static float q3;

//output
float eulerian[6];




int initmahony()
{
	q0=1.0;
	q1=q2=q3=0;

	integralx=integraly=integralz=0;

	return 1;
}
void killmahony()
{
}




//accel + gyro + mag
void mahonyahrsupdate(int N)
{
	float norm,T;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	//value
	float ax=smooth[0];
	float ay=smooth[1];
	float az=smooth[2];
	float gx=smooth[3];
	float gy=smooth[4];
	float gz=smooth[5];
	float mx=smooth[6];
	float my=smooth[7];
	float mz=smooth[8];


	//time
	T=(float)timeinterval / 1000.0 / 1000.0;
	//printf("T=%f\n",T);


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

	// integral error scaled by Ki
	integralx += Ki * halfex * T;
	integraly += Ki * halfey * T;
	integralz += Ki * halfez * T;

	// proportional + integral
	gx += Kp * halfex + integralx;
	gy += Kp * halfey + integraly;
	gz += Kp * halfez + integralz;
	

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
/*
	printf("q:	%f	%f	%f	%f\n",
		q0,q1,q2,q3
	);
*/

	eulerian[3*N + 0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	eulerian[3*N + 1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	eulerian[3*N + 2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;
}
