#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>




//mpu9250.c
extern short measuredata[6];

//kalman.c
extern float predictdata[6];

//quaternion.c
extern float eulerian[3];
extern float eulerianbase[3];

//pid.c
extern int deltaspeed[4];

//main.c
int timeinterval;




static void sig_int(int num)
{
	killmotor();
	killpid();
	killquaternion();
	killkalman();
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

	//control flight
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
		return -5;
	}

	//wait for esc powerup
	gettimeofday(&start,0);
	ret=0;
	while(1)
	{
		//read sensor
		mpu9250();

		//kalman filter
		kalman();

		//update state
		imuupdate();

		//convert value
		pid();

		//time end
		gettimeofday(&end,0);
		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			printf("error@timeinternal\n",timeinterval);
			return;
		}
		else if(timeinterval>6000*1000)
		{
			printf("go\n");
			break;
		}
		else if(timeinterval>ret)
		{
			printf("%ds left\n",6-ret/1000/1000);
			ret+=1000*1000;
		}
	}
	eulerianbase[0]=eulerian[0];
	eulerianbase[1]=eulerian[1];
	eulerianbase[2]=eulerian[2];

	//forever
	memset(&start,0,sizeof(struct timeval));
	memset(&end,0,sizeof(struct timeval));
	while(1)
	{
		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			timeinterval=3000;	//3ms?
		}
		//printf("%d\n",timeinterval);

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

