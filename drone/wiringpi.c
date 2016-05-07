#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softPwm.h>

#define motorleft1  32
#define motorleft2  36
#define motorright1  38
#define motorright2  40
#define powerrelay 37




#define RANGE 300
	int basespeed[4]={100,100,100,100};
extern	int deltaspeed[4];




int initmotor()
{
	//keep the min value
	wiringPiSetupPhys () ;
	softPwmCreate (motorleft1, basespeed[0], RANGE);
	softPwmCreate (motorleft2, basespeed[1], RANGE);
	softPwmCreate (motorright1, basespeed[2], RANGE);
	softPwmCreate (motorright2, basespeed[3], RANGE);

	//power the esc up , wait for "do-re-mi , di---"
	pinMode(powerrelay,OUTPUT);
	digitalWrite(powerrelay,1);

	printf("6\n");
	sleep(1);

	printf("5\n");
	sleep(1);

	printf("4\n");
	sleep(1);

	printf("3\n");
	sleep(1);

	printf("2\n");
	sleep(1);

	printf("1\n");
	sleep(1);

	printf("go\n");
	return 1;
}
int killmotor()
{
	digitalWrite(powerrelay,0);
}
int motor()
{
	int speed1=basespeed[0]+deltaspeed[0];
	int speed2=basespeed[1]+deltaspeed[1];
	int speed3=basespeed[2]+deltaspeed[2];
	int speed4=basespeed[3]+deltaspeed[3];
/*
	printf("%d,	%d,	%d,	%d\n",
		speed1,
		speed2,
		speed3,
		speed4
	);
*/
	//if(speed1>120)speed1=120;
	//if(speed2>120)speed2=120;
	//if(speed3>120)speed3=120;
	//if(speed4>120)speed4=120;
	speed1=speed2=speed3=speed4=135;
	softPwmWrite (motorleft1, speed1);
	softPwmWrite (motorleft2, speed2);
	softPwmWrite (motorright1,speed3);
	softPwmWrite (motorright2,speed4);
}
