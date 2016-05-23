#define BYTE unsigned char
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>




/*
int initpwm(){return 1;}
int killpwm(){}
int pwm(){}
*/

#include <wiringPi.h>
#include <softPwm.h>

#define lefttail  32
#define leftfront  36
#define rightfront  38
#define righttail  40
#define powerrelay 37

//3ms=3,000us=3,000,000ns
#define RANGE 3*1000




//[0,1000*1000]
extern	int deltaspeed[4];

//lefttail,leftfront,rightfront,righttail
int zerospeed[4]={1000,1000,1000,1000};




int initpwm()
{
	int ret;

	//softpwm(very inaccurate)
	softPwmCreate(lefttail, zerospeed[0]/100, RANGE/10);
	softPwmCreate(leftfront, zerospeed[1]/10, RANGE/10);
	softPwmCreate(rightfront, zerospeed[2]/10, RANGE/10);
	softPwmCreate(righttail, zerospeed[3]/10, RANGE/10);

	//power the esc up , wait for "do-re-mi , di---"
	wiringPiSetupPhys () ;
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,1);

	return 1;
}
int killpwm()
{
	int i;

	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,0);
}
int pwm()
{
	int lb=zerospeed[0] + (deltaspeed[0] / 10000);
	int lf=zerospeed[1] + (deltaspeed[1] / 10000);
	int rf=zerospeed[2] + (deltaspeed[2] / 10000);
	int rb=zerospeed[3] + (deltaspeed[3] / 10000);

	//min=100, max=200, limit to 150
	if(lb>150)lb=150;
	if(lf>150)lf=150;
	if(rf>150)rf=150;
	if(rb>150)rb=150;

	//soft................
	softPwmWrite(lefttail,  lb);
	softPwmWrite(leftfront, lf);
	softPwmWrite(rightfront,rf);
	softPwmWrite(righttail, rb);

	//printf("pwm:	%d,%d,%d,%d\n", lb, lf, rf, rb );

}
