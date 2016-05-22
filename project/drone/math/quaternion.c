#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>
#define Kp 100.0f
#define Ki 0.005f
#define beta 10.0f

//
extern int timeinterval;
extern float predictdata[9];

//????
static float integralx;
static float integraly;
static float integralz;

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

	integralx=integraly=integralz=0;

	return 1;
}
void killquaternion()
{
}




//accel + gyro
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

	eulerian[0] = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
	eulerian[1] = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
	eulerian[2] = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;

	printf("imu:	%lf	%lf	%lf\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);

}




//accel + gyro + mag
void mahonyahrsupdate()
{
	float norm,T;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	//time
	T=(float)timeinterval / 1000.0 / 1000.0;
	//printf("T=%f\n",T);

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




/*
//accel + gyro + mag
void madgwickahrsupdate()
{
	float norm,T;
	float hx, hy;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx;
	float _2bx, _2bz;
	float _4bx, _4bz;
	float _2q0, _2q1, _2q2, _2q3;
	float _2q0q2, _2q2q3;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	//time
	T=(float)timeinterval / 1000.0 / 1000.0;
	//printf("T=%f\n",T);

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


	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);


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
	_2q0mx = 2.0f * q0 * mx;
	_2q0my = 2.0f * q0 * my;
	_2q0mz = 2.0f * q0 * mz;
	_2q1mx = 2.0f * q1 * mx;

	_2q0 = 2.0f * q0;
	_2q1 = 2.0f * q1;
	_2q2 = 2.0f * q2;
	_2q3 = 2.0f * q3;

	_2q0q2 = 2.0f * q0 * q2;
	_2q2q3 = 2.0f * q2 * q3;

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
	hx	= mx * q0q0
		- _2q0my * q3
		+ _2q0mz * q2
		+ mx * q1q1
		+ _2q1 * my * q2
		+ _2q1 * mz * q3
		- mx * q2q2
		- mx * q3q3;

	hy	= _2q0mx * q3
		+ my * q0q0
		- _2q0mz * q1
		+ _2q1mx * q2
		- my * q1q1
		+ my * q2q2
		+ _2q2 * mz * q3
		- my * q3q3;

	_2bx	= sqrt(hx * hx + hy * hy);
	_2bz	= -_2q0mx * q2
		+ _2q0my * q1
		+ mz * q0q0
		+ _2q1mx * q3
		- mz * q1q1
		+ _2q2 * my * q3
		- mz * q2q2
		+ mz * q3q3;

	_4bx	= 2.0f * _2bx;
	_4bz	= 2.0f * _2bz;


	// Gradient decent algorithm corrective step
	s0	= -_2q2 * (2.0f * q1q3 - _2q0q2 - ax)
		+ _2q1 * (2.0f * q0q1 + _2q2q3 - ay)
		- _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
		+ (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
		+ _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

	s1	= _2q3 * (2.0f * q1q3 - _2q0q2 - ax)
		+ _2q0 * (2.0f * q0q1 + _2q2q3 - ay)
		- 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az)
		+ _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
		+ (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
		+ (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

	s2	= -_2q0 * (2.0f * q1q3 - _2q0q2 - ax)
		+ _2q3 * (2.0f * q0q1 + _2q2q3 - ay)
		- 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az)
		+ (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
		+ (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
		+ (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

	s3	= _2q1 * (2.0f * q1q3 - _2q0q2 - ax)
		+ _2q2 * (2.0f * q0q1 + _2q2q3 - ay)
		+ (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
		+ (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
		+ _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

	// normalise step magnitude	// Apply feedback step
	norm = sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
	qDot1 -= beta * s0 / norm;
	qDot2 -= beta * s1 / norm;
	qDot3 -= beta * s2 / norm;
	qDot4 -= beta * s3 / norm;

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * T;
	q1 += qDot2 * T;
	q2 += qDot3 * T;
	q3 += qDot4 * T;


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
*/
