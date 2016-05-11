#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
/*
int initmotor(){return 1;}
int killmotor(){}
int motor(){}
*/

#include <wiringPi.h>
#include <softPwm.h>

#define lefttail  32
#define leftfront  36
#define rightfront  38
#define righttail  40

#define powerrelay 37
#define RANGE 300




extern	int motorspeed[4];

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

	return 1;
}
int killmotor()
{
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,0);
}
int motor()
{
	int lb=zerospeed[0]+motorspeed[0];
	int lf=zerospeed[1]+motorspeed[1];
	int rf=zerospeed[2]+motorspeed[2];
	int rb=zerospeed[3]+motorspeed[3];

	//min
	if(lb<100)lb=100;
	if(lf<100)lf=100;
	if(rf<100)rf=100;
	if(rb<100)rb=100;

	//max
	if(lb>150)lb=166;
	if(lf>150)lf=166;
	if(rf>150)rf=166;
	if(rb>150)rb=166;

	softPwmWrite(lefttail,	lb);
	softPwmWrite(leftfront, lf);
	softPwmWrite(rightfront,rf);
	softPwmWrite(righttail,	rb);
/*
	printf("%d,%d,%d,%d\n",
		lb,
		lf,
		rf,
		rb
	);
*/
}
