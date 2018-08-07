#include <stdio.h>
#include <string.h>
#define u64 unsigned long long




//in:	pitch,yaw,roll
extern u64 timeinterval;
extern int thresholdspeed[4];
extern float ebase[3];
extern float eulerian[3];

//out:	lefttail,leftfront,rightfront,righttail
int deltaspeed[4];

//pitch
float pitch_P;
float pitch_I;
float pitch_D;
float pitch_now;
float pitch_before;
float pitch_sum;
float pitch_out;
//yaw
float yaw_P;
float yaw_I;
float yaw_D;
float yaw_now;
float yaw_before;
float yaw_sum;
float yaw_out;
//roll
float roll_P;
float roll_I;
float roll_D;
float roll_now;
float roll_before;
float roll_sum;
float roll_out;




int initpid()
{
	//PID
	pitch_P=2.8;
	pitch_I=0;
	pitch_D=0;
	pitch_now=0;
	pitch_before=0;
	pitch_sum=0;

	roll_P=2.7;
	roll_I=0;
	roll_D=0;
	roll_now=0;
	roll_before=0;
	pitch_sum=0;

	return 1;
}
void killpid()
{
}

//new = P*() + I*() + D*()
int pid()
{
	//initial value
	deltaspeed[0]=thresholdspeed[0];
	deltaspeed[1]=thresholdspeed[1];
	deltaspeed[2]=thresholdspeed[2];
	deltaspeed[3]=thresholdspeed[3];
	if(thresholdspeed[0]<100)goto finishpid;




	//pitch
	pitch_now = ebase[0]-eulerian[0];

	pitch_sum += pitch_now;
	if(pitch_sum > 1000)pitch_sum = 1000;

	pitch_out	= pitch_P * pitch_now
			+ pitch_I * pitch_sum * timeinterval
			+ pitch_D * (pitch_now - pitch_before) / timeinterval;

	pitch_before = pitch_now;

	if(pitch_out>0)
	{
		deltaspeed[1] += (int)pitch_out;
		deltaspeed[2] += (int)pitch_out;
	}
	else
	{
		deltaspeed[0] -= (int)pitch_out;
		deltaspeed[3] -= (int)pitch_out;
	}




	//roll
	roll_now = ebase[2]-eulerian[2];

	roll_sum += roll_now;
	if(roll_sum > 1000)roll_sum = 1000;

	roll_out	= roll_P * roll_now
			+ roll_I * (roll_sum * timeinterval)
			+ roll_D * (roll_now - roll_before)/timeinterval;

	roll_before = roll_now;

	if(roll_out>0)
	{
		deltaspeed[0] += (int)roll_out;
		deltaspeed[1] += (int)roll_out;
	}
	else
	{
		deltaspeed[2] -= (int)roll_out;
		deltaspeed[3] -= (int)roll_out;
	}





	if(deltaspeed[0] < 0)deltaspeed[0] = 0;
	if(deltaspeed[1] < 0)deltaspeed[1] = 0;
	if(deltaspeed[2] < 0)deltaspeed[2] = 0;
	if(deltaspeed[3] < 0)deltaspeed[3] = 0;




finishpid:
/*
	printf("pid:	%f,%f\n",
		pitch_out,
		roll_out
	);
*/
/*
	printf("pid:	%d	%d	%d	%d\n",
		deltaspeed[0],
		deltaspeed[1],
		deltaspeed[2],
		deltaspeed[3]
        );
*/
	return;
}
