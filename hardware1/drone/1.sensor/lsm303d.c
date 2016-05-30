#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

//
static unsigned char reg[0x10];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
extern float measure[20];




int initlsm303d()
{
	//LSM303_REGISTER_ACCEL_CTRL_REG1_A
	reg[0]=0x27;
	systemi2c_write(0x1d,0x20,reg,1);
	usleep(1000);

	//LSM303_REGISTER_MAG_MR_REG_M
	//reg[0]=0;
	//systemi2c_write(0x1d,0x2,reg,1);
	//usleep(1000);




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
	systemi2c_read(0x1d, 0x28, reg, 1);
	systemi2c_read(0x1d, 0x29, reg+1, 1);
	systemi2c_read(0x1d, 0x2a, reg+2, 1);
	systemi2c_read(0x1d, 0x2b, reg+3, 1);
	systemi2c_read(0x1d, 0x2c, reg+4, 1);
	systemi2c_read(0x1d, 0x2d, reg+5, 1);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/





	//
	temp = *(short*)(reg+0x0);
	measure[10] = temp *2* 9.8 / 32768;

	temp=*(short*)(reg+0x2);
	measure[11] = temp *2* 9.8 / 32768;

	temp=*(short*)(reg+0x4);
	measure[12] = temp *2* 9.8 / 32768;




/*
	printf("lsm:	%f	%f	%f\n",
		measure[10],
		measure[11],
		measure[12]
	);
*/
}
