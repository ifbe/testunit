#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>



void motorwrite(int L, int R)
{
	digitalWrite( 3, HIGH);
	digitalWrite(21, L);
	digitalWrite(22, !L);
	digitalWrite(23, L);
	digitalWrite(24, !L);
	digitalWrite(25, HIGH);

	digitalWrite( 5, HIGH);
	digitalWrite( 6, R);
	digitalWrite(26, !R);
	digitalWrite(27, R);
	digitalWrite(28, !R);
	digitalWrite(29, HIGH);
}
void motorstop()
{
	digitalWrite( 3, 0);
	digitalWrite(25, 0);

	digitalWrite( 5, 0);
	digitalWrite(29, 0);
}
void main(int argc, char** argv)
{
	int L,R;
	if(argc < 3)
	{
		printf("./a.out 1 0\n");
		return;
	}

	wiringPiSetup();

	pinMode( 3,OUTPUT);
	pinMode(21,OUTPUT);
	pinMode(22,OUTPUT);
	pinMode(23,OUTPUT);
	pinMode(24,OUTPUT);
	pinMode(25,OUTPUT);

	pinMode( 5,OUTPUT);
	pinMode( 6,OUTPUT);
	pinMode(26,OUTPUT);
	pinMode(27,OUTPUT);
	pinMode(28,OUTPUT);
	pinMode(29,OUTPUT);

	L = argv[1][0] - 0x30;
	R = argv[2][0] - 0x30;
	motorwrite(L, R);

	usleep(2000*1000);
	motorstop();

	pinMode( 3,INPUT);
	pinMode(21,INPUT);
	pinMode(22,INPUT);
	pinMode(23,INPUT);
	pinMode(24,INPUT);
	pinMode(25,INPUT);

	pinMode( 5,INPUT);
	pinMode( 6,INPUT);
	pinMode(26,INPUT);
	pinMode(27,INPUT);
	pinMode(28,INPUT);
	pinMode(29,INPUT);
}