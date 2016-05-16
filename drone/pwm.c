#define BYTE unsigned char
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
int systemi2c_read(BYTE dev,BYTE reg,BYTE* buf,BYTE count);
int systemi2c_write(BYTE dev,BYTE reg,BYTE* buf,BYTE count);




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
int zerospeed[4]={1000,1000,1000,1000};




int initmotor()
{
	int ret;
	unsigned char buf[16];

	//hardwarepwm(pca9685)
        buf[0]=0x31;		//sleep
        systemi2c_write(0x40, 0, buf, 1);

        buf[0]=0x20;		//prescale
        systemi2c_write(0x40, 0xef, buf, 1);

        buf[0]=0xa1;		//wake
        systemi2c_write(0x40, 0, buf, 1);

        buf[0]=0x4;		//restart
        systemi2c_write(0x40, 1, buf, 1);

	buf[0]=0;		//T=3ms
	buf[1]=0;
	buf[2]=3000&0xff;
	buf[3]=3000>>8;
	systemi2c_write(0x40, 6, buf, 4);

	//softpwm(very inaccurate)
	//softPwmCreate(lefttail, zerospeed[0]/1000, RANGE/1000);
	//softPwmCreate(leftfront, zerospeed[1]/1000, RANGE/1000);
	//softPwmCreate(rightfront, zerospeed[2]/1000, RANGE/1000);
	//softPwmCreate(righttail, zerospeed[3]/1000, RANGE/1000);

	//power the esc up , wait for "do-re-mi , di---"
	wiringPiSetupPhys () ;
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
	int lb=zerospeed[0] + (deltaspeed[0] / 500);
	int lf=zerospeed[1] + (deltaspeed[1] / 500);
	int rf=zerospeed[2] + (deltaspeed[2] / 500);
	int rb=zerospeed[3] + (deltaspeed[3] / 500);

	//min=1000, max=3000, limit to 2000
	if(lb>2000)lb=2000;
	if(lf>2000)lf=2000;
	if(rf>2000)rf=2000;
	if(rb>2000)rb=2000;

	//softPwmWrite(lefttail,  lb);
	//softPwmWrite(leftfront, lf);
	//softPwmWrite(rightfront,rf);
	//softPwmWrite(righttail, rb);

	printf("%d,%d,%d,%d\n", lb, lf, rf, rb );

}
