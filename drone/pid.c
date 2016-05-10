#include <stdio.h>
#include <string.h>




//in:	pitch,yaw,roll
extern float eulerianbase[3];
extern float eulerian[3];
extern int timeinterval;

//out:	lefttail,leftfront,rightfront,righttail
int thresholdspeed[4];
int motorspeed[4];

//pid's plaything
float P[2];
float I[2];
float D[2];
float err_now[2];
float err_before[2];
float err_sum[2];




int initpid()
{
	//lefttail,leftfront,rightfront,righttail
	thresholdspeed[0]=39;
	thresholdspeed[1]=39;
	thresholdspeed[2]=39;
	thresholdspeed[3]=39;

	//PID
	P[0]=P[1]=0.8;
	I[0]=I[1]=0;
	D[0]=D[1]=0;

	//err
	err_now[0]=err_now[1]=0;
	err_before[0]=err_before[1]=0;
	err_sum[0]=err_sum[1]=0;

	return 1;
}
void killpid()
{
}

//new = P*() + I*() + D*()
int pid()
{
	float temp1,temp2;

	//pitch
	err_now[0]	= eulerianbase[0]-eulerian[0];
	err_sum[0]	+=err_now[0];
	temp1	= P[0]*(err_now[0])
		+ I[0]*(err_sum[0]*timeinterval)
		+ D[0]*(err_now[0]-err_before[0])/timeinterval;

	//roll
	err_now[1]	= eulerianbase[2]-eulerian[2];
	err_sum[1]	+=err_now[1];
	temp2	= P[1]*(err_now[1])
		+ I[1]*(err_sum[1]*timeinterval)
		+ D[1]*(err_now[1]-err_before[1])/timeinterval;

	//
	err_before[0]	= err_now[0];
	err_before[1]	= err_now[1];
	//printf("%f,%f\n",temp1,temp2);

	//
	motorspeed[0]=thresholdspeed[0];
	motorspeed[1]=thresholdspeed[1];
	motorspeed[2]=thresholdspeed[2];
	motorspeed[3]=thresholdspeed[3];
	if(temp1>0)
	{
		motorspeed[1] += (int)temp1;
		motorspeed[2] += (int)temp1;
	}
	else
	{
		motorspeed[0] -= (int)temp1;
		motorspeed[3] -= (int)temp1;
	}

	if(temp2>0)
	{
		motorspeed[0] += (int)temp2;
		motorspeed[1] += (int)temp2;
	}
	else
	{
		motorspeed[2] -= (int)temp2;
		motorspeed[3] -= (int)temp2;
	}
}
