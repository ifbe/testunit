#include <stdio.h>
#include <string.h>




//in:	pitch,yaw,roll
extern int timeinterval;
extern int thresholdspeed[4];
extern float eulerianbase[3];
extern float eulerian[3];

//out:	lefttail,leftfront,rightfront,righttail
int deltaspeed[4];

//pid's plaything
float P[4];
float I[4];
float D[4];
float err_now[4];
float err_before[4];
float err_sum[4];




int initpid()
{
	//PID
	P[0]=2.8;
	P[1]=2.78;
	P[2]=1.9;
	P[3]=1.8;

	I[0]=0;
	I[1]=0;
	I[2]=0;
	I[3]=0;

	D[0]=0;
	D[1]=0;
	D[2]=0;
	D[3]=0;

	//err
	err_now[0]=0;
	err_now[0]=0;
	err_now[0]=0;
	err_now[0]=0;

	err_before[0]=0;
	err_before[1]=0;
	err_before[2]=0;
	err_before[3]=0;

	err_sum[0]=0;
	err_sum[1]=0;
	err_sum[2]=0;
	err_sum[3]=0;

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
	if(err_sum[0] > 1000)err_now[0]=1000;

	temp1	= P[0]*(err_now[0])
		+ I[0]*(err_sum[0]*timeinterval)
		+ D[0]*(err_now[0]-err_before[0])/timeinterval;

	//roll
	err_now[1]	= eulerianbase[2]-eulerian[2];
	err_sum[1]	+=err_now[1];
	if(err_sum[1] > 1000)err_now[1]=1000;

	temp2	= P[1]*(err_now[1])
		+ I[1]*(err_sum[1]*timeinterval)
		+ D[1]*(err_now[1]-err_before[1])/timeinterval;

	//
	err_before[0]	= err_now[0];
	err_before[1]	= err_now[1];
	//printf("%f,%f\n",temp1,temp2);

	//
	deltaspeed[0]=thresholdspeed[0];
	deltaspeed[1]=thresholdspeed[1];
	deltaspeed[2]=thresholdspeed[2];
	deltaspeed[3]=thresholdspeed[3];
	if(temp1>0)
	{
		deltaspeed[1] += (int)temp1;
		deltaspeed[2] += (int)temp1;
	}
	else
	{
		deltaspeed[0] -= (int)temp1;
		deltaspeed[3] -= (int)temp1;
	}

	if(temp2>0)
	{
		deltaspeed[0] += (int)temp2;
		deltaspeed[1] += (int)temp2;
	}
	else
	{
		deltaspeed[2] -= (int)temp2;
		deltaspeed[3] -= (int)temp2;
	}
/*
	printf("%d,%d,%d,%d\n",
		deltaspeed[0],
		deltaspeed[1],
		deltaspeed[2],
		deltaspeed[3]
        );
*/
}
