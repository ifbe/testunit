#define BYTE unsigned char
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>

//
static int ac1;
static int ac2;
static int ac3;
static int ac4;
static int ac5;
static int ac6;
static int b1;
static int b2;
static int mb;
static int mc;
static int md;

//
static unsigned char reg[0x16];

//
float measure[2];
float temperature;

#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3
#define _bmp085Mode BMP085_STANDARD



int initbmp180()
{
	//
	systemi2c_read(0x77,0xaa,reg,0x16);

	//
	ac1=*(signed short*)(reg+0);
	ac2=*(signed short*)(reg+2);
	ac3=*(signed short*)(reg+4);
	ac4=*(signed short*)(reg+6);
	ac5=*(signed short*)(reg+8);
	ac6=*(signed short*)(reg+10);
	b1=*(signed short*)(reg+12);
	b2=*(signed short*)(reg+14);
	mb=*(signed short*)(reg+16);
	mc=*(signed short*)(reg+18);
	md=*(signed short*)(reg+20);

printf(	"ac1=%d\n"
	"ac2=%d\n"
	"ac3=%d\n"
	"ac4=%d\n"
	"ac5=%d\n"
	"ac6=%d\n"
	"b1=%d\n"
	"b2=%d\n"
	"mb=%d\n"
	"mc=%d\n"
	"md=%d\n",

	ac1,ac2,ac3,ac4,ac5,ac6,b1,b2,mb,mc,md
);
	//
	return 1;
}
void killbmp180()
{
}




int readbmp180()
{
	long long temp,comp;
	long long ii,jj,p;
	long long x1,x2,x3;
	long long b3,b5,b6;
	unsigned long long b4,b7;




	//command	//2e->temperature, 34->pressure
	reg[0]=0x2e;
	systemi2c_write(0x77, 0xf4, reg,1);

	//wait		//low->5ms, standard->8ms, high->14ms, ultra->26ms
	usleep(5000);

	//data		//temperature->2byte, pressure->3byte
	systemi2c_read(0x77, 0xf6, reg, 2);

	temp = (reg[0]<<8) + (reg[1]);
	temp >>= (8 - _bmp085Mode);




	//command	//2e->temperature, 34->pressure
	reg[0]=0x34 + (_bmp085Mode << 6);
	systemi2c_write(0x77, 0xf4, reg,1);

	//wait		//low->5ms, standard->8ms, high->14ms, ultra->26ms
	usleep(8000);

	//data		//temperature->2byte, pressure->3byte
	systemi2c_read(0x77, 0xf6, reg, 3);

	comp = (reg[0]<<16) + (reg[1]<<8) + (reg[2]);
	comp >>= (8 - _bmp085Mode);
//printf("%d	%d	%d\n",reg[0],reg[1],reg[2]);




	ii = ( (temp - ac6) * ac5 ) >> 15;
	jj = (mc << 11) / (ii + md) - 4000;
	b6 = ii + jj;

	x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = ( ( (ac1 * 4 + x3) << _bmp085Mode) + 2) >> 2;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (x3 + 32768)) >> 15;
	b7 = ((comp - b3) * (50000 >> _bmp085Mode));

	if (b7 < 0x80000000){p = (b7 << 1) / b4;}
	else{p = (b7 / b4) << 1;}

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;

	jj = p + ((x1 + x2 + 3791) >> 4);

	measure[0] = jj;
	//measure[0] = 44330 * ( 1.0 - pow(jj / 256.0 / 101325.0 , 0.1903) );




print:
	printf("%lld	%1.1f\n",temp,measure[0]);
	return;
}

void main()
{
	systemi2c_init();
	initbmp180();

	while(1)
	{
		readbmp180();
	}
}
