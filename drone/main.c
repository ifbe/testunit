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
extern int thresholdspeed[4];
extern int motorspeed[4];

//main.c
int timeinterval;
int alive=666666;




static void sig_int(int num)
{
	killmotor();
	killpid();
	killquaternion();
	killkalman();
	killmpu9250();

	exit(-1);
}

static void keyboardthread()
{
	while(1)
	{
		scanf("%d\n",&alive);
	}
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
	pthread_t control=0;
	struct timeval start,end;

	//how to die
	signal(SIGINT,sig_int);

	//control thread
	ret = pthread_create (&control, NULL, keyboardthread, NULL);
	if (ret != 0)
	{
		printf("fail@pthread_create\n");
		return 0;
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
			printf("error@timeinternal\n",timeinterval);
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
	eulerianbase[0]=eulerian[0];
	eulerianbase[1]=eulerian[1];
	eulerianbase[2]=eulerian[2];

starting:
	//wait for esc powerup
	haha=0;
	gettimeofday(&start,0);
	while(1)
	{
		//time start
		gettimeofday(&end,0);

		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		//printf("%d\n",timeinterval);

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
		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			printf("error@timeinternal\n",timeinterval);
			goto cutpower;
		}
		else if(timeinterval>1500*1000)
		{
			printf("bye\n");
			break;
		}
		else if(timeinterval>haha)
		{
			ret=haha/50/1000;
			thresholdspeed[0]=thresholdspeed[1]=thresholdspeed[2]=thresholdspeed[3]=ret;
			printf("speed=%d\n",ret);
			haha+=500*1000;
		}

		//time end
		memcpy( &end, &start, sizeof(struct timeval) );
	}
	thresholdspeed[0]=thresholdspeed[1]=thresholdspeed[2]=thresholdspeed[3]=40;

going:
	//forever
	gettimeofday(&start,0);
	while(1)
	{
		//time start
		gettimeofday(&end,0);

		//stop?
		if(alive != 666666)break;

		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		//printf("%d\n",timeinterval);

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

landing:
	//wait for esc powerup
	haha=0;
	gettimeofday(&start,0);
	while(1)
	{
		//time start
		gettimeofday(&end,0);

		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		//printf("%d\n",timeinterval);

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
		timeinterval=timeval_subtract(&start,&end);
		if(timeinterval<=0)
		{
			printf("error@timeinternal\n",timeinterval);
			goto cutpower;
		}
		else if(timeinterval>1500*1000)
		{
			printf("bye\n");
			break;
		}
		else if(timeinterval>haha)
		{
			ret=30-haha/50/1000;
			thresholdspeed[0]=thresholdspeed[1]=thresholdspeed[2]=thresholdspeed[3]=ret;
			printf("speed=%d\n",ret);
			haha+=500*1000;
		}

		//time end
		memcpy( &end, &start, sizeof(struct timeval) );
	}

cutpower:
	//bye bye
	sig_int(666666);
}
