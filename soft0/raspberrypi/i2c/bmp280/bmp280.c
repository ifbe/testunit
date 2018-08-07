#define BYTE unsigned char
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>

//
static unsigned char reg[0x18];
static int dig_T1;
static int dig_T2;
static int dig_T3;
static int dig_P1;
static int dig_P2;
static int dig_P3;
static int dig_P4;
static int dig_P5;
static int dig_P6;
static int dig_P7;
static int dig_P8;
static int dig_P9;

//[0,9]:	(ax,ay,az),(gx,gy,gz),(mx,my,mz),(baro)
//[10,19]:	(ax,ay,az),(gx,gy,gz),(mx,my,mz),(baro)
float measure[2];
float temperature;




int initbmp280()
{
	//CTRL_REG1
	reg[0]=0x3f;
	systemi2c_write(0x76,0xf4,reg,1);

	systemi2c_read(0x76,0x88,reg,0x18);
	dig_T1=*(unsigned short*)(reg+0);
	dig_T2=*(short*)(reg+2);
	dig_T3=*(short*)(reg+4);
	dig_P1=*(unsigned short*)(reg+6);
	dig_P2=*(short*)(reg+8);
	dig_P3=*(short*)(reg+0xa);
	dig_P4=*(short*)(reg+0xc);
	dig_P5=*(short*)(reg+0xe);
	dig_P6=*(short*)(reg+0x10);
	dig_P7=*(short*)(reg+0x12);
	dig_P8=*(short*)(reg+0x14);
	dig_P9=*(short*)(reg+0x16);

	//
	usleep(4000);
	return 1;
}
void killbmp280()
{
}




int readbmp280()
{
	long long temp;
	long long var1,var2;
	long long t,p;

	//0xc.0x3b -> 0x20
	systemi2c_read(0x76, 0xf7, reg, 6);
/*
	for(temp=0;temp<6;temp++)
	{
		printf("%x ",reg[temp]);
	}
	printf("\n");
*/

	//temperature
	temp = reg[0x3];
	temp = (temp<<8) + reg[0x4];
	temp = (temp<<8) + reg[0x5];
	temp >>= 4;
	var1 = ( ( (temp >> 3) - (dig_T1 << 1) ) * dig_T2 ) >> 11;
	temp = (temp>>4) - dig_T1;	//no long used
	var2 = ( ( (temp*temp) >> 12) * dig_T3 ) >> 14;
	t=var1+var2;
	temperature =( t * 5 + 128) / 25600.0;
//printf("temp=%f\n",temperature);

	//pressure
	temp = reg[0x0];
	temp = (temp<<8) + reg[0x1];
	temp = (temp<<8) + reg[0x2];
	temp >>= 4;

	var1 = t - 128000;
	var2 = var1 * var1 * (long long)dig_P6;
	var2 = var2 + ((var1 * (long long)dig_P5) << 17);
	var2 = var2 + (((long long)dig_P4) << 35);
	var1	= ((var1 * var1 * (long long)dig_P3) >> 8)
		+ ((var1 * (long long)dig_P2) << 12);
	var1 = ((((long long)1)<<47)+var1) * ((long long)dig_P1) >> 33;
	if (var1 == 0){measure[0]=0;goto print;}	//avoid zero

	p = 1048576 - temp;
	p = (((p<<31)-var2)*3125)/var1;
//printf("%lld	%lld	%lld\n",var1,var2,p);
	var1 = (((long long)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((long long)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((long long)dig_P7)<<4);

	measure[0] = 44330 * ( 1.0 - pow(p / 256.0 / 101325.0 , 0.1903) );

print:
	printf("%1.3f\n",measure[0]);
	return;
}

void main()
{
	systemi2c_init();
	initbmp280();

	while(1)
	{
		readbmp280();
	}
}
