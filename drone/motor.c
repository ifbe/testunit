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

//3ms=3,000us=3,000,000ns
#define RANGE 3*1000*1000




//[0,1000*1000]
extern	int deltaspeed[4];

//lefttail,leftfront,rightfront,righttail
int zerospeed[4]={1000*1000,1000*1000,1000*1000,1000*1000};




void pwmcreate(int pin,int duty_nano_second,int period_nano_second)
{
	softPwmCreate(pin, duty_nano_second/10000, period_nano_second/10000);
	//systemi2c_write(dev,reg,buf,count);
}
void pwmwrite(int pin,int duty_nano_second)
{
	softPwmWrite(pin, duty_nano_second/10000);
	//systemi2c_write(dev,reg,buf,count);
}

int initmotor()
{
	//power the esc up , wait for "do-re-mi , di---"
	wiringPiSetupPhys () ;
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,1);

	//softpwm(very inaccurate)
	pwmcreate (lefttail, zerospeed[0], RANGE);
	pwmcreate (leftfront, zerospeed[1], RANGE);
	pwmcreate (rightfront, zerospeed[2], RANGE);
	pwmcreate (righttail, zerospeed[3], RANGE);

	//hardwarepwm(pca9685)
	//systemi2c_write(0xe0,reg,buf,count);

	return 1;
}
int killmotor()
{
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,0);
}
int motor()
{
	int lb=zerospeed[0]+deltaspeed[0];
	int lf=zerospeed[1]+deltaspeed[1];
	int rf=zerospeed[2]+deltaspeed[2];
	int rb=zerospeed[3]+deltaspeed[3];

	//min=1.0ms
	if(lb<1000*1000)lb=1000*1000;
	if(lf<1000*1000)lf=1000*1000;
	if(rf<1000*1000)rf=1000*1000;
	if(rb<1000*1000)rb=1000*1000;

	//max=1.5ms
	if(lb>1500*1000)lb=1500*1000;
	if(lf>1500*1000)lf=1500*1000;
	if(rf>1500*1000)rf=1500*1000;
	if(rb>1500*1000)rb=1500*1000;

	pwmwrite(lefttail,  lb);
	pwmwrite(leftfront, lf);
	pwmwrite(rightfront,rf);
	pwmwrite(righttail, rb);
/*
	printf("%d,%d,%d,%d\n",
		lb,
		lf,
		rf,
		rb
	);
*/
}
