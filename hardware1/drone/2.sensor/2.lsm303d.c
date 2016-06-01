#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

//
static float xmin=0;
static float xmax=0;
static float ymin=0;
static float ymax=0;
static float zmin=0;
static float zmax=0;

//
static unsigned char reg[0x10];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
extern float measure[20];




int initlsm303d()
{
	//CTRL1
	reg[0]=0x57;
	systemi2c_write(0x1d,0x20,reg,1);
	usleep(1000);

        //LSM303_CTRL2
        reg[0]=0x00;
        systemi2c_write(0x1d,0x21,reg,1);
        usleep(1000);

        //LSM303_CTRL4
        reg[0]=4<<2;
        systemi2c_write(0x1d,0x23,reg,1);
        usleep(1000);

        //LSM303_CTRL5
        reg[0]=0x64;
        systemi2c_write(0x1d,0x24,reg,1);
        usleep(1000);

        //LSM303_CTRL6
        reg[0]=0x20;
        systemi2c_write(0x1d,0x25,reg,1);
        usleep(1000);

        //LSM303_CTRL7
        reg[0]=0x0;
        systemi2c_write(0x1d,0x26,reg,1);
        usleep(1000);




	//
	return 1;
}
void killlsm303d()
{
}




int readlsm303d()
{
	int temp;

	//0xc.0x3b -> 0x20
	systemi2c_read(0x1d, 0x8, reg+0, 1);
	systemi2c_read(0x1d, 0x9, reg+1, 1);
	systemi2c_read(0x1d, 0xa, reg+2, 1);
	systemi2c_read(0x1d, 0xb, reg+3, 1);
	systemi2c_read(0x1d, 0xc, reg+4, 1);
	systemi2c_read(0x1d, 0xd, reg+5, 1);
	systemi2c_read(0x1d, 0x28, reg+6, 1);
	systemi2c_read(0x1d, 0x29, reg+7, 1);
	systemi2c_read(0x1d, 0x2a, reg+8, 1);
	systemi2c_read(0x1d, 0x2b, reg+9, 1);
	systemi2c_read(0x1d, 0x2c, reg+10, 1);
	systemi2c_read(0x1d, 0x2d, reg+11, 1);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/





	//
	temp = *(short*)(reg+0x6);
	measure[10] = temp * 9.8 / 16384.0;

	temp=*(short*)(reg+0x8);
	measure[11] = temp * 9.8 / 16384.0;

	temp=*(short*)(reg+0xa);
	measure[12] = temp * 9.8 / 16384.0;

	temp = *(short*)(reg+0x0);
	measure[16] = temp;

	temp=*(short*)(reg+0x2);
	measure[17] = temp;

	temp=*(short*)(reg+0x4);
	measure[18] = temp;




/*
	printf("lsm:	%f	%f	%f	%f	%f	%f\n",
		measure[10],
		measure[11],
		measure[12]
	);
*/
}
