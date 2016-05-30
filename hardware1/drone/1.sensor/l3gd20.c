#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

//l3gd20's plaything
static unsigned char reg[0x10];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
extern float measure[20];




int initl3gd20()
{
	//CTRL_REG1
	reg[0]=0xf;
	systemi2c_write(0x6b,0x20,reg,1);
	usleep(1000);

	//CTRL_REG4
	reg[0]=0x20;	//2000dps
	systemi2c_write(0x6b,0x23,reg,1);
	usleep(1000);




	//
	return 1;
}
void killl3gd20()
{
}




int readl3gd20()
{
	int temp;

	//0xc.0x3b -> 0x20
	systemi2c_read(0x6b, 0x28, reg, 1);
	systemi2c_read(0x6b, 0x29, reg+1, 1);
	systemi2c_read(0x6b, 0x2a, reg+2, 1);
	systemi2c_read(0x6b, 0x2b, reg+3, 1);
	systemi2c_read(0x6b, 0x2c, reg+4, 1);
	systemi2c_read(0x6b, 0x2d, reg+5, 1);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/





	//mag
	temp = *(short*)(reg+0x0);
	measure[13] = temp / 16.4 / 57.3;	//2000dps?

	temp=*(short*)(reg+0x2);
	measure[14] = temp / 16.4 / 57.3;

	temp=*(short*)(reg+0x4);
	measure[15] = temp / 16.4 / 57.3;




/*
	printf("l3gd20:	%f	%f	%f\n",
		measure[13],
		measure[14],
		measure[15]
	);
*/
}
