#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>




//out1:		pitch,yaw,roll
//out2:		basespeed
float eulerianbase[3];
int thresholdspeed[4];

//............
static struct termios stored_settings;




//new thread
static void* keyboardthread(void* in)
{
	int ret;
	while(1)
	{
		ret=getchar();
		if(ret=='\033')
		{
			getchar();	//'['
			ret=getchar();

			if(ret==65)
			{
				if(thresholdspeed[0]<40)
				{
					thresholdspeed[0]++;
					thresholdspeed[1]++;
					thresholdspeed[2]++;
					thresholdspeed[3]++;
					printf("threshold=%d\n",thresholdspeed[0]);
				}
			}
			else if(ret==66)
			{
				if(thresholdspeed[0]>0)
				{
					thresholdspeed[0]--;
					thresholdspeed[1]--;
					thresholdspeed[2]--;
					thresholdspeed[3]--;
					printf("threshold=%d\n",thresholdspeed[0]);
				}
			}
		}
	}
}




int initcontrol()
{
	int ret;
	struct termios new_settings;
	pthread_t control=0;

	//control thread
	ret = pthread_create (&control, NULL, keyboardthread, NULL);
	if (ret != 0)
	{
		printf("fail@pthread_create\n");
		return 0;
	}

	//
	tcgetattr(0,&stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_lflag &= (~ECHO);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&new_settings);

	//angle
	eulerianbase[0]=0;
	eulerianbase[1]=0;
	eulerianbase[2]=0;

        //speed
	thresholdspeed[0]=0;
	thresholdspeed[1]=0;
	thresholdspeed[2]=0;
	thresholdspeed[3]=0;

	return 1;
}

void killcontrol()
{
	tcsetattr(0,TCSANOW,&stored_settings);
}
