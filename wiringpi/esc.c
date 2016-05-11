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

#define range 3000
	//default	10us		1us
	//10->1ms	100->1ms	1000->1ms
	//20->2ms	200->2ms	2000->2ms
	//100->10ms	1000->10ms	10000->10ms
	//200->20ms	2000->20ms	20000->20ms
	//1000->100ms	10000->100ms	100000->100ms

int main(int argc,char** argv)
{
	char input[128];
	int speed;

	wiringPiSetupPhys();
	softPwmCreate(lefttail, 2000, range);
	softPwmCreate(leftfront, 2000, range);
	softPwmCreate(rightfront, 2000, range);
	softPwmCreate(righttail, 2000, range);

	//normal mode:	1000,power
	//setup mode:	2000,power,1000,[1000,2000]
	while(1)
	{
		scanf("%s",&input);
		if(input[0]=='q')break;
		else speed=atoi(input);

		printf("speed=%d\n",speed);
		softPwmWrite (lefttail,speed);
		softPwmWrite (leftfront,speed);
		softPwmWrite (rightfront,speed);
		softPwmWrite (righttail,speed);
	}
} 
