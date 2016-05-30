#include <stdio.h>
#include <string.h>




//input
extern float measure[10+10];

//kalman's plaything
static float H;
static float R;
static float A;
static float Q;
static float gain[10+10];
static float predictp[10+10];

//output
float smooth[10+10];




int initkalman()
{
	smooth[0] = 0;
	smooth[1] = 0;
	smooth[2] = 9.8;

	predictp[0] = 1;
	predictp[1] = 1;
	predictp[2] = 1;

	A = 1;		//testonly=1
	H = 1;
	Q = 1;		//10e-3;	/* predict noise convariance */
	R = 50;		//10e-2;	/* measure error convariance */
}
void killkalman()
{
}
int kalman()
{
	int j;
/*
	for(j=0;j<20;j++)
	{
		smooth[j]=measure[j];
	}
	goto printdata;
*/

	for(j=0;j<20;j++)
	{
		//data		p(n|n-1)=A^2*p(n-1|n-1)+q
		//smooth[j] = A * smooth[j];

		//possibility
		predictp[j] = A*A*predictp[j] + Q;

		//Measurement
		gain[j] = H*predictp[j] / (H*H*predictp[j] + R);

		//gain
		smooth[j] +=	gain[j] * (measure[j] - H * smooth[j]);

		//p
		predictp[j] = (1 - H*gain[j]) * predictp[j];
	}

printdata:
	printf("kalman:	%f	%f	%f	%f\n",
		smooth[0],
		smooth[1],
		smooth[2],
		smooth[9]
	);

	printf("kalman:	%f	%f	%f	%f\n",
		smooth[10],
		smooth[11],
		smooth[12],
		smooth[19]
	);

	return 0;
}
