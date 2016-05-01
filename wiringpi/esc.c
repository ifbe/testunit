#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softPwm.h>

#define pin 37
#define range 400

int main(int argc,char** argv)
{
	char input[128];
	int speed;

	wiringPiSetupPhys();
	softPwmCreate(pin, 0, range);

	//normal mode:	160,power
	//setup mode:	320,power,160,[160,320]
	while(1)
	{
		scanf("%s",&input);
		if(input[0]=='q')break;
		else speed=atoi(input);

		printf("speed=%d\n",speed);
		softPwmWrite (pin,speed);
	}
} 
