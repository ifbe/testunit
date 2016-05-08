#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softPwm.h>

#define lefttail  32
#define leftfront  36
#define rightfront  38
#define righttail  40

#define powerrelay 37
#define RANGE 300




extern	int deltaspeed[4];

//lefttail,leftfront,rightfront,righttail
int zerospeed[4]={100,100,100,100};




int initmotor()
{
	//keep the min value
	wiringPiSetupPhys () ;
	softPwmCreate (lefttail, zerospeed[0], RANGE);
	softPwmCreate (leftfront, zerospeed[1], RANGE);
	softPwmCreate (rightfront, zerospeed[2], RANGE);
	softPwmCreate (righttail, zerospeed[3], RANGE);

	//power the esc up , wait for "do-re-mi , di---"
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,1);
}
int killmotor()
{
	digitalWrite(powerrelay,0);
}
int motor()
{
	int lb=zerospeed[0]+deltaspeed[0];
	int lf=zerospeed[1]+deltaspeed[1];
	int rf=zerospeed[2]+deltaspeed[2];
	int rb=zerospeed[3]+deltaspeed[3];

	if(lb>150)lb=150;
	if(lf>150)lf=150;
	if(rf>150)rf=150;
	if(rb>150)rb=150;

	softPwmWrite(lefttail,	lb);
	softPwmWrite(leftfront, lf);
	softPwmWrite(rightfront,rf);
	softPwmWrite(righttail,	rb);
}
