#include<wiringPi.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
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

void main()
{
	int i;
	struct timeval start,end;

	wiringPiSetupPhys();
	pinMode(31,OUTPUT);
	pinMode(33,OUTPUT);
	pinMode(35,OUTPUT);
	pinMode(37,OUTPUT);

	gettimeofday(&start,0);

	for(i=0;i<250;i++)
	{
		digitalWrite(31,LOW);
		digitalWrite(33,LOW);
		digitalWrite(35,LOW);
		digitalWrite(37,LOW);
	}

	gettimeofday(&end,0);
	printf("%d\n",timeval_subtract(&start,&end));
}
