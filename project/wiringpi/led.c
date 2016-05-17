#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<softPwm.h>
#include<unistd.h>

void displaycolor(int r,int g,int b)
{
	unsigned int color=0;
	softPwmWrite( 33, r );
	softPwmWrite( 35, g );
	softPwmWrite( 37, b );
}
void main()
{
	int i;
	int fangxiang=1;
	int r=64;
	int g=64;
	int b=64;

	wiringPiSetupPhys();
	softPwmCreate( 33, 64, 64 );
	softPwmCreate( 35, 64, 64 );
	softPwmCreate( 37, 64, 64 );

	while(1)
	{
		displaycolor(r,g,b);
		for(i=0;i<200;i++)usleep(1);

		//blue
		if(fangxiang==1)
		{
			b=(b-1)&0x3f;
			if(b==0)fangxiang=-1;
		}
		if(fangxiang==-1)
		{
			b=(b+1)&0x3f;
			if(b==0x3f)fangxiang=2;
		}

		//green
		if(fangxiang==2)
		{
			g=(g-1)&0x3f;
			if(g==0)fangxiang=-2;
		}
		if(fangxiang==-2)
		{
			g=(g+1)&0x3f;
			if(g==0x3f)fangxiang=4;
		}

		//red
		if(fangxiang==4)
		{
			r=(r-1)&0x3f;
			if(r==0)fangxiang=-4;
		}
		if(fangxiang==-4)
		{
			r=(r+1)&0x3f;
			if(r==0x3f)fangxiang=0x3;
		}

		//blue+green
		if(fangxiang==0x3)
		{
			b=(b-1)&0x3f;
			g=(g-1)&0x3f;
			if(b==0)fangxiang=-0x3;
		}
		if(fangxiang==-0x3)
		{
			b=(b+1)&0x3f;
			g=(g+1)&0x3f;
			if(b==0x3f)fangxiang=5;
		}

		//blue+red
		if(fangxiang==0x5)
		{
			b=(b-1)&0x3f;
			r=(r-1)&0x3f;
			if(b==0)fangxiang=-0x5;
		}
		if(fangxiang==-0x5)
		{
			b=(b+1)&0x3f;
			r=(r+1)&0x3f;
			if(b==0x3f)fangxiang=6;
		}

		//green+red
		if(fangxiang==0x6)
		{
			g=(g-1)&0x3f;
			r=(r-1)&0x3f;
			if(r==0)fangxiang=-0x6;
		}
		if(fangxiang==-0x6)
		{
			g=(g+1)&0x3f;
			r=(r+1)&0x3f;
			if(r==0x3f)fangxiang=7;
		}

		//white
		if(fangxiang==0x7)
		{
			b=(b-1)&0x3f;
			g=(g-1)&0x3f;
			r=(r-1)&0x3f;
			if(r==0)fangxiang=-0x7;
		}
		if(fangxiang==-0x7)
		{
			b=(b+1)&0x3f;
			g=(g+1)&0x3f;
			r=(r+1)&0x3f;
			if(r==0x3f)fangxiang=1;
		}
/*
		if(b==0)
		{
			g=(g+1)&0xff;
			if(g==0)
			{
				r=(r+1)&0xff;
			}
		}
*/
	}
}
