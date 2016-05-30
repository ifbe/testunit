#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>




//mpu6050's plaything
static unsigned char reg[0x20];

//
extern float measure[30];




int initmpu6050()
{
	//PWR_MGMT_1	reset
	reg[0]=0x80;
	systemi2c_write(0x69,0x6b,reg,1);
	usleep(1000);

	//PWR_MGMT_1	clock
	reg[0]=0x01;
	systemi2c_write(0x69,0x6b,reg,1);
	usleep(1000);

	//PWR_MGMT_2	enable
	reg[0]=0x00;
	systemi2c_write(0x69,0x6c,reg,1);
	usleep(1000);

	//CONFIG
	reg[0]=0x3;
	systemi2c_write(0x69,0x1a,reg,1);
	usleep(1000);

	//SMPLRT_DIV
	reg[0]=0x4;
	systemi2c_write(0x69,0x19,reg,1);
	usleep(1000);

	//GYRO_CONFIG
	systemi2c_read(0x69,0x1b,reg,1);
	reg[0] &= 0xe5;
	reg[0] |= (3<<3);
	systemi2c_write(0x69,0x1b,reg,1);
	usleep(1000);

	//ACCEL_CONFIG
	systemi2c_read(0x69,0x1c,reg,1);
	reg[0] &= 0xe7;
	reg[0] |= (1<<3);
	systemi2c_write(0x69,0x1c,reg,1);
	usleep(1000);

	//ACCEL_CONFIG2
	systemi2c_read(0x69,0x1d,reg,1);
	reg[0] &= 0xf0;
	reg[0] |= 0x3;
	systemi2c_write(0x69,0x1d,reg,1);
	usleep(1000);

	//INT_PIN_CFG
	reg[0]=0x22;
	systemi2c_write(0x69,0x37,reg,1);
	usleep(1000);

	//INT_ENABLE
	reg[0]=0x1;
	systemi2c_write(0x69,0x38,reg,1);
	usleep(1000);




	//
	return 1;
}
void killmpu6050()
{
}




int readmpu6050()
{
	int temp;

	//0x69.0x3b -> 0x00
	systemi2c_read(0x69, 0x3b, reg+0, 14);




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	measure[20] = temp * 9.8 / 8192.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	measure[21] = temp * 9.8 / 8192.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	measure[22] = temp * 9.8 / 8192.0;




	//temp
	//temp=(reg[6]<<8) + reg[7];
	//temp6050 = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	measure[23] = temp / 16.4 / 57.3;

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	measure[24] = temp / 16.4 / 57.3;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	measure[25] = temp / 16.4 / 57.3;




/*
	printf("6050:	%f	%f	%f\n",
		measure[20],
		measure[21],
		measure[22]
	);
*/
/*
	printf("6050:	%f	%f	%f\n",
		measure[23],
		measure[24],
		measure[25]
	);
*/
}
