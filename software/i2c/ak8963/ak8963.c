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

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
float measure[3];




int initak8963()
{
	//AK8963_CNT2
	buf[0]=0x1;
	systemi2c_write(0xc,0xb,buf,1);
	usleep(1000);

	//AK8963_CNT2
	buf[0]=0x16;
	systemi2c_write(0xc,0xa,buf,1);
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





	printf("8963:	%f	%f	%f\n",
		measure[0],
		measure[1],
		measure[2]
	);

}




void main()
{
	systemi2c_init();
	initak8963();

	while(1)
	{
		readak8963();
	}
}
