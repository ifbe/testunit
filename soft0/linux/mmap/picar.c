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

int xy2lr(float x, float y, float* l, float* r)
{
	l[0] = r[0] = y;
	l[0] += x*0.6;
	r[0] -= x*0.6;
	return 0;
}

int lr2ed(float l, float r, int* le, int* ld, int* re, int* rd)
{
	if((l > -0.2)&&(l < 0.2)){
		le[0] = 0;
		ld[0] = 0;
	}
	else if(l < -0.5){
		le[0] = 1;
		ld[0] = 0;
	}
	else if(l > 0.5){
		le[0] = 1;
		ld[0] = 1;
	}
	else{
		le[0] = 0;
		ld[0] = 0;
	}

	if((r > -0.2)&&(r < 0.2)){
		re[0] = 0;
		rd[0] = 0;
	}
	else if(r < -0.5){
		re[0] = 1;
		rd[0] = 0;
	}
	else if(r > 0.5){
		re[0] = 1;
		rd[0] = 1;
	}
	else{
		re[0] = 0;
		rd[0] = 0;
	}
	return 0;
}
int move(void* virtual, int le, int ld, int re, int rd)
{
	//rf
	setvolt(virtual,16,!rd);
	setvolt(virtual,20, rd);
	setvolt(virtual,21, re);	//en
	//rn
	setvolt(virtual,24, re);	//en
	setvolt(virtual,25,!rd);
	setvolt(virtual,12, rd);

	//lf
	setvolt(virtual,22, le);	//en
	setvolt(virtual, 5,!ld);
	setvolt(virtual, 6, ld);
	//ln
	setvolt(virtual,13,!ld);
	setvolt(virtual,19, ld);
	setvolt(virtual,26, le);	//en

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

	move(virtual,-1,1);
	usleep(1*1000*1000);

	move(virtual, 1,1);
	usleep(1*1000*1000);
*/
	float x,y;
	float l,r;
	int le,ld,re,rd;

	printf("l\n");
	x =-1;
	y = 0;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("r\n");
	x = 1;
	y = 0;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("f\n");
	x = 0;
	y = 1;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("n\n");
	x = 0;
	y =-1;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("lf\n");
	x =-0.7;
	y = 0.7;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("rn\n");
	x = 0.7;
	y =-0.7;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("ln\n");
	x =-0.7;
	y =-0.7;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
	usleep(1*1000*1000);

	printf("rf\n");
	x = 0.7;
	y = 0.7;
	printf("x=%f,y=%f\n", x, y);
	xy2lr(x, y, &l, &r);
	printf("l=%f,r=%f\n", l, r);
	lr2ed(l, r, &le, &ld, &re, &rd);
	printf("le=%d,ld=%d,re=%d,rd=%d\n", le, ld, re, rd);
	move(virtual, le,ld, re,rd);
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
