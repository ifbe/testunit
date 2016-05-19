#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>




//main.c
int timeinterval;




static void sig_int(int num)
{
	killmotor();
	killpid();
	killcontrol();
	killquaternion();
	killkalman();
	killmpu9250();
	killlibrary();

	exit(-1);
}

int timeval_subtract(struct timeval* x, struct timeval* y)   
{
	int tv_sec;
	int tv_usec;

	if( x->tv_sec>y->tv_sec ) return -1;
	if( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec) ) return -1;   
 
	tv_sec = ( y->tv_sec-x->tv_sec );
	tv_usec = ( y->tv_usec-x->tv_usec );
	//printf("%d,%d\n",tv_sec,tv_usec);

	if(tv_usec<0)   
	{   
		tv_sec--;   
		tv_usec+=1000000;   
	}   
 
	return tv_usec+(tv_sec*1000000);   
}

int main(int argc,char** argv)
{
	int ret;
	int haha;
	struct timeval start,end;

	ret=getuid();
	if(ret!=0)
	{
		printf("please run as root\n");
		return 0;
	}

	//how to die
	signal(SIGINT,sig_int);

	//user control
	ret=initcontrol();
	if(ret<=0)
	{
		printf("fail@initcontrol\n");
	}

	//system library
	ret=initlibrary();
	if(ret<=0)
	{
		printf("fail@initlibrary\n");
		return -1;
	}

	//mpu9250 initialization
	ret=initmpu9250();
	if(ret<=0)
	{
		printf("fail@initmpu9250\n");
		return -1;
	}

	//data filter
	ret=initkalman();
	if(ret<=0)
	{
		printf("fail@initkalman\n");
		return -2;
	}

	//calculate state
	ret=initquaternion();
	if(ret<=0)
	{
		printf("fail@initquaternion\n");
		return -3;
	}

	//keep stable
	ret=initpid();
	if(ret<=0)
	{
		printf("fail@initpid\n");
		return -4;
	}

	//relay and esc
	ret=initmotor();
	if(ret<=0)
	{
		printf("fail@initmotor\n");
		goto cutpower;
	}

going:
	//forever
	gettimeofday(&start,0);
	usleep(5000);
	while(1)
	{
		//time start
		gettimeofday(&end,0);

		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		//printf("time:	%d\n",timeinterval);

		//read sensor
		mpu9250();

		//kalman filter
		kalman();

		//update state
		//imuupdate();
		//madgwickahrsupdate();
		mahonyahrsupdate();

		//convert value
		pid();

		//write pwm
		motor();

		//time end
		start.tv_sec = end.tv_sec;
		start.tv_usec = end.tv_usec;
	}

cutpower:
	//bye bye
	sig_int(666666);
}
