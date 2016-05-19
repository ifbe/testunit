#define BYTE unsigned char
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

/*
int initmpu9250(){return 1;}
int killmpu9250(){}
int mpu9250(){usleep(1000);}
*/
short xmin=-139;
short xmax=243;
short ymin=87;
short ymax=472;
short zmin=-146;
short zmax=294;





//mpu9250's plaything
unsigned char reg[0x40];

//(ax,ay,az),(gx,gy,gz),(mx,my,mz),(temp)
float measuredata[10];




int initmpu9250()
{
	//PWR_MGMT_1	reset
	reg[0]=0x80;
	systemi2c_write(0x68,0x6b,reg,1);

	//PWR_MGMT_1	clock
	reg[0]=0x01;
	systemi2c_write(0x68,0x6b,reg,1);

	//PWR_MGMT_2	enable
	reg[0]=0x00;
	systemi2c_write(0x68,0x6c,reg,1);

	//CONFIG
	reg[0]=0x3;
	systemi2c_write(0x68,0x1a,reg,1);

	//SMPLRT_DIV
	reg[0]=0x4;
	systemi2c_write(0x68,0x19,reg,1);

	//GYRO_CONFIG
	systemi2c_read(0x68,0x1b,reg,1);
	reg[0] &= 0xe5;
	reg[0] |= (3<<3);
	systemi2c_write(0x68,0x1b,reg,1);

	//ACCEL_CONFIG
	systemi2c_read(0x68,0x1c,reg,1);
	reg[0] &= 0xe7;
	reg[0] |= (1<<3);
	systemi2c_write(0x68,0x1c,reg,1);

	//ACCEL_CONFIG2
	systemi2c_read(0x68,0x1d,reg,1);
	reg[0] &= 0xf0;
	reg[0] |= 0x3;
	systemi2c_write(0x68,0x1d,reg,1);

	//INT_PIN_CFG
	reg[0]=0x22;
	systemi2c_write(0x68,0x37,reg,1);

	//INT_ENABLE
	reg[0]=0x1;
	systemi2c_write(0x68,0x38,reg,1);




	//AK8963_CNT2
	reg[0]=0x1;
	systemi2c_write(0xc,0xb,reg,1);

	//AK8963_CNT2
	reg[0]=0x16;
	systemi2c_write(0xc,0xa,reg,1);




	//
	return 1;
}
void killmpu9250()
{
}




int mpu9250()
{
	int temp;

	//0x68.0x3b -> 0x00
	systemi2c_read(0x68, 0x3b, reg+0, 14);

	//0xc.0x3b -> 0x20
	systemi2c_read(0xc, 3, reg+0x23, 7);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[0x23+temp]);
	}
	printf("\n");
*/




	//accel
	temp=(reg[0]<<8) + reg[1];
	if(temp>32768)temp = temp-65536;
	measuredata[0] = temp * 9.82 / 8192.0;

	temp=(reg[2]<<8) + reg[3];
	if(temp>32768)temp = temp-65536;
	measuredata[1] = temp * 9.82 / 8192.0;

	temp=(reg[4]<<8) + reg[5] +1;		//ensure not 0
	if(temp>32768)temp = temp-65536;
	measuredata[2] = temp * 9.82 / 8192.0;




	//temp
	temp=(reg[6]<<8) + reg[7];
	measuredata[9] = temp / 100.0;




	//gyro
	temp=(reg[ 8]<<8) + reg[ 9];
	if(temp>32768)temp = temp-65536;
	measuredata[3] = temp * 180.0 / 32768.0;

	temp=(reg[10]<<8) + reg[11];
	if(temp>32768)temp = temp-65536;
	measuredata[4] = temp * 180.0 / 32768.0;

	temp=(reg[12]<<8) + reg[13];
	if(temp>32768)temp = temp-65536;
	measuredata[5] = temp * 180.0 / 32768.0;




	//mag
	temp = *(short*)(reg+0x23);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (xmin+xmax)/2;
	measuredata[6] = temp * 4912.0 / 32760.0;

	temp=*(short*)(reg+0x25);
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (ymin+ymax)/2;
	measuredata[7] = temp * 4912.0 / 32760.0;

	temp=*(short*)(reg+0x27)	+1;	//ensure not 0
	//if(temp<zmin)zmin=temp;
	//if(temp>zmax)zmax=temp;
	temp = temp - (zmin+zmax)/2;
	measuredata[8] = temp * 4912.0 / 32760.0;
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
	printf("9250:	%f	%f	%f\n",
		measuredata[0],
		measuredata[1],
		measuredata[2]
	);
*/
/*
	printf("9250:	%f	%f	%f\n",
		measuredata[3],
		measuredata[4],
		measuredata[5]
	);
*/
/*
	printf("9250:	%f	%f	%f\n",
		measuredata[6],
		measuredata[7],
		measuredata[8]
	);
*/
	//printf("9250:	%f\n",measuredata[9]);
}
