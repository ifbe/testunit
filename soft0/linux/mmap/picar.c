#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

//altfunc
#define GPFSEL0   0x00
#define GPFSEL1   0x04
#define GPFSEL2   0x08
#define GPFSEL3   0x0C
#define GPFSEL4   0x10
#define GPFSEL5   0x14
//output
#define GPSET0    0x1C
#define GPSET1    0x20
#define GPCLR0    0x28
#define GPCLR1    0x2c

#define GPIOACCESS(x) (*(volatile unsigned int*)(gpio+x))

/*
		case hex32('l','f','e',0):pin =  5;	//24
		case hex32('l','f','p',0):pin = 21;	//5
		case hex32('l','f','n',0):pin = 22;	//6

		case hex32('l','n','p',0):pin = 23;	//13
		case hex32('l','n','n',0):pin = 24;	//19
		case hex32('l','n','e',0):pin = 29;	//21

		case hex32('r','f','e',0):pin = 25;
		case hex32('r','f','p',0):pin =  6;
		case hex32('r','f','n',0):pin = 26;

		case hex32('r','n','p',0):pin = 27;
		case hex32('r','n','n',0):pin = 28;
		case hex32('r','n','e',0):pin =  3;
*/

char* val2str[8] = {
	"input","output","alt5","alt4",
	"alt0", "alt1",  "alt2","alt3"
};

int setfunc(void* gpio, int pin, int func)
{
	if(pin < 0)return 0;
	if(pin > 45)return 0;

	int hh = pin/10;
	int ll = pin%10;

	unsigned int val = GPIOACCESS(GPFSEL0 + hh*4);
	val &= ~(3 << (ll*3));
	val |= func << (ll*3);
	GPIOACCESS(GPFSEL0 + hh*4) = val;
	return 1;
}

int setvolt(void* gpio, int pin, int volt)
{
	if(pin < 0)return 0;
	if(pin > 45)return 0;

	unsigned int offs = (pin/32)*4;
	if(1 == volt){
		offs += GPSET0;
	}
	else{
		offs += GPCLR0;
	}
	GPIOACCESS(offs) = 1<<(pin%32);
	return 1;
}

int move(void* virtual, int x, int y)
{
	int l = y;
	int r = y;
	if(x<0){
		l = 0;
	}
	if(x>0){
		r = 0;
	}

	//rf
	setvolt(virtual,16,!r);
	setvolt(virtual,20, r);
	setvolt(virtual,21, 1);	//en
	//rn
	setvolt(virtual,24, 1);	//en
	setvolt(virtual,25,!r);
	setvolt(virtual,12, r);

	//lf
	setvolt(virtual,22, 1);	//en
	setvolt(virtual, 5,!l);
	setvolt(virtual, 6, l);
	//ln
	setvolt(virtual,13,!l);
	setvolt(virtual,19, l);
	setvolt(virtual,26, 1);	//en

	return 0;
}

int main()
{
	int _dev_mem=open("/dev/gpiomem",O_RDWR|O_SYNC);
	printf("open:%x\n",_dev_mem);

	unsigned int* virtual=mmap(
                NULL,
                0x1000,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                _dev_mem,
                0
	);
	printf("mmap:%llx\n",virtual);

	int x,y;
	unsigned int val;
	for(y=0;y<5;y++){
		val = virtual[y];
		printf("GPFSEL%d=%x\n", y, val);
		for(x=0;x<10;x++){
			printf("%d: func=%d(%s)\n", y*10+x, val&7, val2str[val&7]);
			val >>= 3;
		}
	}

	while(1){
		printf("%c\n",getchar());
	}

	//rf
	setfunc(virtual,16, 1);
	setfunc(virtual,20, 1);
	setfunc(virtual,21, 1);		//en

	//rn
	setfunc(virtual,24, 1);		//en
	setfunc(virtual,25, 1);
	setfunc(virtual,12, 1);

	//lf
	setfunc(virtual,22, 1);		//en
	setfunc(virtual, 5, 1);
	setfunc(virtual, 6, 1);

	//ln
	setfunc(virtual,13, 1);
	setfunc(virtual,19, 1);
	setfunc(virtual,26, 1);		//en

/*
	move(virtual, 0,1);
	usleep(1*1000*1000);

	move(virtual, 0,0);
	usleep(1*1000*1000);
*/
	move(virtual,-1,1);
	usleep(1*1000*1000);

	move(virtual, 1,1);
	usleep(1*1000*1000);


	setfunc(virtual,16, 0);
	setfunc(virtual,20, 0);
	setfunc(virtual,21, 0);

	setfunc(virtual,24, 0);
	setfunc(virtual,25, 0);
	setfunc(virtual,12, 0);

	setfunc(virtual,22, 0);
	setfunc(virtual, 5, 0);
	setfunc(virtual, 6, 0);

	setfunc(virtual,13, 0);
	setfunc(virtual,19, 0);
	setfunc(virtual,26, 0);

	return 0;
}
