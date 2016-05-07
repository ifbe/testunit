#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>




//
int timedifference;




static void sig_int(int num)
{
	killmotor();
	killkalman();
	killpid();
	killmpu9250();

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
	struct timeval start,end;

	//how to die
	signal(SIGINT,sig_int);

	//init time
	memset(&start,0,sizeof(struct timeval));
	memset(&end,0,sizeof(struct timeval));

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
		return -2;
	}

	//control flight
	ret=initpid();
	if(ret<=0)
	{
		printf("fail@initpid\n");
		return -3;
	}

	//relay and esc
	ret=initmotor();
	if(ret<=0)
	{
		printf("fail@initmotor\n");
		return -4;
	}

	//forever
	while(1)
	{
		timedifference=timeval_subtract(&start,&end);
		if(timedifference<=0)
		{
			timedifference=3000;	//3ms?
		}
		//printf("%d\n",timedifference);

		//time start
		gettimeofday(&start,0);

		//read sensor
		mpu9250();

		//kalman filter
		kalman();

		//update state
		imuupdate();

		//convert value
		pid();

		//write pwm
		motor();

		//time end
		gettimeofday(&end,0);
	}
}

