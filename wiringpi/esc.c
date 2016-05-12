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

#define range 3*1000*1000
	//default	10us		1ns
	//10->1ms	100->1ms	1,000,000->1ms
	//20->2ms	200->2ms	2,000,000->2ms
	//100->10ms	1000->10ms	10,000,000->10ms
	//200->20ms	2000->20ms	20,000,000->20ms
	//1000->100ms	10000->100ms	100,000,000->100ms

void pwmcreate(int pin,int duty_nano_second,int period_nano_second)
{
	softPwmCreate(pin, duty_nano_second/10000, period_nano_second/10000);
}
void pwmwrite(int pin,int duty_nano_second)
{
	softPwmWrite(pin, duty_nano_second/10000);
}
int main(int argc,char** argv)
{
	char input[128];
	int speed;

	//duty=2ms=2,000us=2,000,000ns, period=3ms=3,000us=3,000,000ns
	wiringPiSetupPhys();
	pwmcreate(lefttail,  2*1000*1000, range);
	pwmcreate(leftfront, 2*1000*1000, range);
	pwmcreate(rightfront,2*1000*1000, range);
	pwmcreate(righttail, 2*1000*1000, range);

	//normal mode:	duty=1ms,power
	//setup mode:	duty=2ms,power,duty=1ms,[1ms,2ms]
	while(1)
	{
		//get input
		scanf("%s",&input);
		if(input[0]=='q')break;

		//parse input
		speed=atoi(input);
		if(speed<1000*1000)speed=1000*1000;
		printf("speed=%d\n",speed);

		//send output
		pwmwrite(lefttail,speed);
		pwmwrite(leftfront,speed);
		pwmwrite(rightfront,speed);
		pwmwrite(righttail,speed);
	}
} 
