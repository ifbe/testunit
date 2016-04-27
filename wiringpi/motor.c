#include<wiringPi.h>
#include<stdio.h>
#include<stdlib.h>
void main()
{
	int i;
	int speed=50;
	char a[6][3]={
		{HIGH,LOW,LOW},
		{HIGH,HIGH,LOW},
		{LOW,HIGH,LOW},
		{LOW,HIGH,HIGH},
		{LOW,LOW,HIGH},
		{HIGH,LOW,HIGH},
	};

	wiringPiSetupPhys();
	pinMode(31,OUTPUT);
	pinMode(33,OUTPUT);
	pinMode(35,OUTPUT);
	pinMode(37,OUTPUT);
	digitalWrite(31,LOW);
	digitalWrite(33,LOW);
	digitalWrite(35,LOW);
	digitalWrite(37,LOW);

	while(1)
	{
		for(i=0;i<6;i++)
		{
			digitalWrite(31,a[i][0]);
			digitalWrite(33,a[i][1]);
			digitalWrite(37,a[i][2]);
			delay(speed);
		}
		if(speed>5)speed--;
	}
}
