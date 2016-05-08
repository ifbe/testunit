#include <stdio.h>
#include <string.h>




//in:	pitch,yaw,roll
extern float eulerian[3];

//out:	lefttail,leftfront,rightfront,righttail
int deltaspeed[4];
int lastspeed[4];

//pid's plaything
float P;
float I;
float D;




int initpid()
{
	//lefttail,leftfront,rightfront,righttail
	deltaspeed[0]=39;
	deltaspeed[1]=39;
	deltaspeed[2]=39;
	deltaspeed[3]=39;

	//PID
	P=10;
	I=0;
	D=0;

	return 1;
}
void killpid()
{
}
int pid()
{
/*
	//new = P*() + I*() + D*()
	err_now=eulerian[0]-eulerianhistory[0];
	deltaspeed[0]	= P*(err_now-err_before)
			+ I*(err_now)
			+ D*(err_now-2*err_before+err_oldest);
*/
}
