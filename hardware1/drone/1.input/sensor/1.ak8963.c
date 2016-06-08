#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

static int xmin=-139;
static int xmax=243;
static int ymin=87;
static int ymax=472;
static int zmin=-146;
static int zmax=294;





//mpu9250's plaything
static unsigned char magreg[0x10];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
extern float measure[20];




int initak8963()
{
	//AK8963_CNT2
	magreg[0]=0x1;
	systemi2c_write(0xc,0xb,magreg,1);
	usleep(1000);

	//AK8963_CNT2
	magreg[0]=0x16;
	systemi2c_write(0xc,0xa,magreg,1);
	usleep(1000);




	//
	return 1;
}
void killak8963()
{
}




int readak8963()
{
	int temp;

	//0xc.0x3b -> 0x20
	systemi2c_read(0xc, 3, magreg+0x3, 7);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",magreg[0x3+temp]);
	}
	printf("\n");
*/




	//mag
	temp = *(short*)(magreg+0x3);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (xmin+xmax)/2;
	measure[6] = temp * 4912.0 / 32760.0;

	temp=*(short*)(magreg+0x5);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (ymin+ymax)/2;
	measure[7] = temp * 4912.0 / 32760.0;

	temp=*(short*)(magreg+0x7)	+1;	//ensure not 0
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (zmin+zmax)/2;
	measure[8] = temp * 4912.0 / 32760.0;
/*
	printf("%d	%d	%d	%d	%d	%d\n",
		xmin,
		xmax,
		ymin,
		ymax,
		zmin,
		zmax
	);
*/




/*
	printf("8963:	%f	%f	%f\n",
		measure[6],
		measure[7],
		measure[8]
	);
*/
}
