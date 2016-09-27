#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#define u64 unsigned long long




//main.c
u64 timeinterval;




static void sig_int(int num)
{
	killpwm();
	killpid();

	killmahony();
	killimuupdate();

	killkalman();

	//killmpu6050();

	killbmp280();
	killak8963();
	killmpu9250();

	killbmp180();
	killl3gd20();
	killlsm303d();

	killlibrary();
	killcontrol();

	exit(-1);
}
u64 gettime()
{
        struct timeval t;
        gettimeofday(&t,0);
        return (t.tv_sec)*1000*1000 + (t.tv_usec);
}
int main(int argc,char** argv)
{
	int ret,haha;
	u64 start,end;

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

	//mpu6050
	//ret=initmpu6050();
	//if(ret<=0)
	//{
	//	printf("fail@initmpu6050\n");
	//	return -1;
	//}

	//(accel@0 + gyro@3)mpu9250
	ret=initmpu9250();
	if(ret<=0)
	{
		printf("fail@initmpu9250\n");
		return -1;
	}

	//(mag@6)ak8963
	ret=initak8963();
	if(ret<=0)
	{
		printf("fail@initak8963\n");
		return -1;
	}

	//(baro@9)bmp280
	ret=initbmp280();
	if(ret<=0)
	{
		printf("fail@initbmp280\n");
		return -1;
	}

	//(accel@10+mag@16)lsm303
	ret=initlsm303d();
	if(ret<=0)
	{
		printf("fail@initlsm303d\n");
		return -1;
	}

	//(gyro@13)l3gd20
	ret=initl3gd20();
	if(ret<=0)
	{
		printf("fail@initl3gd20\n");
		return -1;
	}

	//(baro@19)bmp180
	ret=initbmp180();
	if(ret<=0)
	{
		printf("fail@initbmp180\n");
		return -1;
	}

	//kalman filter
	ret=initkalman();
	if(ret<=0)
	{
		printf("fail@initkalman\n");
		return -2;
	}

	//imuupdate
	ret=initimuupdate();
	if(ret<=0)
	{
		printf("fail@initimuupdate\n");
		return -3;
	}

	//mahony
	ret=initmahony();
	if(ret<=0)
	{
		printf("fail@initmahony\n");
		return -2;
	}

	//keep stable
	ret=initpid();
	if(ret<=0)
	{
		printf("fail@initpid\n");
		return -4;
	}

	//relay and esc
	ret=initpwm();
	if(ret<=0)
	{
		printf("fail@initpwm\n");
		goto cutpower;
	}

going:
	//forever
	start = gettime();
	usleep(5000);
	while(1)
	{
		//time start
		end=gettime();

		timeinterval=end-start;
		if(timeinterval<=0)
		{
			goto cutpower;
		}
		//printf("time:	%d\n",timeinterval);

		//read sensor
		//readmpu6050();
		readmpu9250();
		readak8963();
		readbmp280();
		readl3gd20();
		readlsm303d();
		readbmp180();

		//kalman filter
		kalman();

		//update state
		imuupdate(0);
		imuupdate(1);
		state();

		//convert value
		pid();

		//write pwm
		pwm();

		//time end
		start = end;
	}

cutpower:
	//bye bye
	sig_int(666666);
}
