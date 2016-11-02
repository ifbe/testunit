#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

short xmin=-139;
short xmax=243;
short ymin=87;
short ymax=472;
short zmin=-146;
short zmax=294;





//mpu9250's plaything
static unsigned char buf[0x10];

//
float measure[3];




int inithmc5983()
{
	//
	buf[0]=0x1;
	systemi2c_write(0xc,0xb,buf,1);
	usleep(1000);

	//
	return 1;
}
void killhmc5983()
{
}




int readhmc5983()
{
	int temp;

	//0xc.0x3b -> 0x20
	systemi2c_read(0xc, 3, buf+0x3, 7);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",buf[0x3+temp]);
	}
	printf("\n");
*/




	//mag
	temp = *(short*)(buf+0x3);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (xmin+xmax)/2;
	measure[0] = temp * 4912.0 / 32760.0;

	temp=*(short*)(buf+0x5);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (ymin+ymax)/2;
	measure[1] = temp * 4912.0 / 32760.0;

	temp=*(short*)(buf+0x7)	+1;	//ensure not 0
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (zmin+zmax)/2;
	measure[2] = temp * 4912.0 / 32760.0;





	printf("5983:	%f	%f	%f\n",
		measure[0],
		measure[1],
		measure[2]
	);

}



void main()
{
	systemi2c_init();
	inithmc5983();

	while(1)
	{
		readhmc5983();
	}
}
