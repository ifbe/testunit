#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softPwm.h>

#define pin 37
#define range 300
	//default	increased
	//10->1ms	100->1ms
	//30->3ms	300->3ms
	//100->10ms	1000->10ms
	//200->20ms	2000->20ms
	//1000->100ms	10000->100ms

int main(int argc,char** argv)
{
	char input[128];
	int speed;

	wiringPiSetupPhys();
	softPwmCreate(pin, 0, range);

	//normal mode:	100,power
	//setup mode:	200,power,100,[100,200]
	while(1)
	{
		scanf("%s",&input);
		if(input[0]=='q')break;
		else speed=atoi(input);

		printf("speed=%d\n",speed);
		softPwmWrite (pin,speed);
	}
} 
