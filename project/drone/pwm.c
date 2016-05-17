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

//
static unsigned char sendbuf[16];




int initmotor()
{
	int ret;

	//hardwarepwm(pca9685)
	sendbuf[0]=0x31;	//sleep
	systemi2c_write(0x40, 0, sendbuf, 1);

	sendbuf[0]=18;		//prescale
	systemi2c_write(0x40, 0xfe, sendbuf, 1);

	sendbuf[0]=0xa1;	//wake
	systemi2c_write(0x40, 0, sendbuf, 1);

	sendbuf[0]=0x4;		//restart
	systemi2c_write(0x40, 1, sendbuf, 1);

	sendbuf[0]=0;		//T=3ms
	sendbuf[1]=0;
	sendbuf[2]=1000&0xff;
	sendbuf[3]=1000>>8;

	sendbuf[4]=0;		//T=3ms
	sendbuf[5]=0;
	sendbuf[6]=1000&0xff;
	sendbuf[7]=1000>>8;

	sendbuf[8]=0;		//T=3ms
	sendbuf[9]=0;
	sendbuf[10]=1000&0xff;
	sendbuf[11]=1000>>8;

	sendbuf[12]=0;		//T=3ms
	sendbuf[13]=0;
	sendbuf[14]=1000&0xff;
	sendbuf[15]=1000>>8;
	systemi2c_write(0x40, 6, sendbuf, 16);

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
	int i;

	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,0);

	for(i=0;i<16;i++)sendbuf[i]=0;
	systemi2c_write(0x40, 6, sendbuf, 16);
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

	//soft................
	//softPwmWrite(lefttail,  lb);
	//softPwmWrite(leftfront, lf);
	//softPwmWrite(rightfront,rf);
	//softPwmWrite(righttail, rb);

	//hard..................
	sendbuf[2]=lb&0xff;
	sendbuf[3]=lb>>8;

	sendbuf[6]=lf&0xff;
	sendbuf[7]=lf>>8;

	sendbuf[10]=rf&0xff;
	sendbuf[11]=rf>>8;

	sendbuf[14]=rb&0xff;
	sendbuf[15]=rb>>8;
	systemi2c_write(0x40, 8, sendbuf+2, 14);

	printf("%d,%d,%d,%d\n", lb, lf, rf, rb );

}
