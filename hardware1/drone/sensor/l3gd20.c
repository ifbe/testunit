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
extern float measure2[20];




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
	systemi2c_read(0x6b, 0x28, reg, 6);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/




	//mag
	temp = *(short*)(reg+0x0);
	measure2[3] = temp * 4912.0 / 32760.0;

	temp=*(short*)(reg+0x2);
	measure2[4] = temp * 4912.0 / 32760.0;

	temp=*(short*)(reg+0x4);	//ensure not 0
	measure2[5] = temp * 4912.0 / 32760.0;




/*
	printf("l3gd20:	%f	%f	%f\n",
		measure2[3],
		measure2[4],
		measure2[5]
	);
*/
}
