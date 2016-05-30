#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>




//mpu9250's plaything
unsigned char reg[0x20];

//
extern float measure[20];




int initmpu9250()
{
	//PWR_MGMT_1	reset
	reg[0]=0x80;
	systemi2c_write(0x68,0x6b,reg,1);
	usleep(1000);

	//PWR_MGMT_1	clock
	reg[0]=0x01;
	systemi2c_write(0x68,0x6b,reg,1);
	usleep(1000);

	//PWR_MGMT_2	enable
	reg[0]=0x00;
	systemi2c_write(0x68,0x6c,reg,1);
	usleep(1000);

	//CONFIG
	reg[0]=0x3;
	systemi2c_write(0x68,0x1a,reg,1);
	usleep(1000);

	//SMPLRT_DIV
	reg[0]=0x4;
	systemi2c_write(0x68,0x19,reg,1);
	usleep(1000);

	//GYRO_CONFIG
	systemi2c_read(0x68,0x1b,reg,1);
	reg[0] &= 0xe5;
	reg[0] |= (3<<3);
	systemi2c_write(0x68,0x1b,reg,1);
	usleep(1000);

	//ACCEL_CONFIG
	systemi2c_read(0x68,0x1c,reg,1);
	reg[0] &= 0xe7;
	reg[0] |= (1<<3);
	systemi2c_write(0x68,0x1c,reg,1);
	usleep(1000);

	//ACCEL_CONFIG2
	systemi2c_read(0x68,0x1d,reg,1);
	reg[0] &= 0xf0;
	reg[0] |= 0x3;
	systemi2c_write(0x68,0x1d,reg,1);
	usleep(1000);

	//INT_PIN_CFG
	reg[0]=0x22;
	systemi2c_write(0x68,0x37,reg,1);
	usleep(1000);

	//INT_ENABLE
	reg[0]=0x1;
	systemi2c_write(0x68,0x38,reg,1);
	usleep(1000);




	//
	return 1;
}
void killmpu9250()
{
}




int readmpu9250()
{
	int temp;

	//0x68.0x3b -> 0x00
	systemi2c_read(0x68, 0x3b, reg+0, 14);




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	measure[0] = temp * 9.82 / 8192.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	measure[1] = temp * 9.82 / 8192.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	measure[2] = temp * 9.82 / 8192.0;




	//temp
	//temp=(reg[6]<<8) + reg[7];
	//temp9250 = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	measure[3] = temp / 57.3  / 16.4;		//2000dps?

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	measure[4] = temp / 57.3 / 16.4;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	measure[5] = temp / 57.3 / 16.4;




/*
	printf("9250:	%f	%f	%f\n",
		measure[0],
		measure[1],
		measure[2]
	);

	printf("9250:	%f	%f	%f\n",
		measure[3],
		measure[4],
		measure[5]
	);

*/
}
