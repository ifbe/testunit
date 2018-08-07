#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE 1000
#define motorleft1  29
#define motorleft2  31
#define motorright1  33
#define motorright2  35

int main(int argc,char** argv)
{
	int i;
	int speed=atoi(argv[1]);

	wiringPiSetupPhys () ;
	softPwmCreate (motorleft1, 0, RANGE);
	softPwmCreate (motorleft2, 0, RANGE);
	softPwmCreate (motorright1, 0, RANGE);
	softPwmCreate (motorright2, 0, RANGE);

	i=0;
	while(1)
	{
		softPwmWrite (motorleft1, i) ;
		softPwmWrite (motorleft2, i) ;
		softPwmWrite (motorright1,i) ;
		softPwmWrite (motorright2,i) ;

		usleep(100);
		if(i<speed)i++;
	}
} 
