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

	//how to die
	signal(SIGINT,sig_int);

	ret=getuid();
	if(ret!=0)
	{
		printf("please run as root\n");
		return 0;
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

	//user control
	ret=initcontrol();
	if(ret<=0)
	{
		printf("fail@initcontrol\n");
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

	//wait for esc powerup
	haha=0;
	gettimeofday(&start,0);
	while(1)
	{
		//read sensor
		mpu9250();

		//kalman filter
		kalman();

		//update state
		imuupdate();

		//time end
		gettimeofday(&end,0);
		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			printf("error@timeinternal=%d\n",timeinterval);
			goto cutpower;
		}
		else if(timeinterval>6000*1000)
		{
			printf("go\n");
			break;
		}
		else if(timeinterval>haha)
		{
			printf("%ds left\n",6-haha/1000/1000);
			haha += 1000*1000;
		}
	}

going:
	//forever
	gettimeofday(&start,0);
	usleep(1000);
	while(1)
	{
		//time start
		gettimeofday(&end,0);

		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		printf("%d\n",timeinterval);

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
		memcpy( &end, &start, sizeof(struct timeval) );
	}

cutpower:
	//bye bye
	sig_int(666666);
}
