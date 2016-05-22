#include <stdio.h>
#include <string.h>




//input
static	float H;
static	float R;
static	float kalmangain[9];
extern	float measuredata[9];

//output
static	float A;
static	float Q;
static	float predictp[9];
	float predictdata[9];




int initkalman()
{
	predictdata[0] = 0;
	predictdata[1] = 0;
	predictdata[2] = 9.8;

	predictdata[3] = 0;
	predictdata[4] = 0;
	predictdata[5] = 0;

	predictdata[6] = 1;
	predictdata[7] = 0;
	predictdata[8] = 0;

	predictp[0] = predictp[1] = predictp[2] = 1;
	predictp[3] = predictp[4] = predictp[5] = 1;
	predictp[6] = predictp[7] = predictp[8] = 1;

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
/*
	predictdata[0]=measuredata[0];
	predictdata[1]=measuredata[1];
	predictdata[2]=measuredata[2];
	predictdata[3]=measuredata[3];
	predictdata[4]=measuredata[4];
	predictdata[5]=measuredata[5];
	predictdata[6]=measuredata[6];
	predictdata[7]=measuredata[7];
	predictdata[8]=measuredata[8];
	goto printdata;
*/




	/* Predict		p(n|n-1)=A^2*p(n-1|n-1)+q */
	predictdata[0] = A * predictdata[0];
	predictdata[1] = A * predictdata[1];
	predictdata[2] = A * predictdata[2];
	predictdata[3] = A * predictdata[3];
	predictdata[4] = A * predictdata[4];
	predictdata[5] = A * predictdata[5];
	predictdata[6] = A * predictdata[6];
	predictdata[7] = A * predictdata[7];
	predictdata[8] = A * predictdata[8];

	predictp[0] = A*A*predictp[0] + Q;
	predictp[1] = A*A*predictp[1] + Q;
	predictp[2] = A*A*predictp[2] + Q;
	predictp[3] = A*A*predictp[3] + Q;
	predictp[4] = A*A*predictp[4] + Q;
	predictp[5] = A*A*predictp[5] + Q;
	predictp[6] = A*A*predictp[6] + Q;
	predictp[7] = A*A*predictp[7] + Q;
	predictp[8] = A*A*predictp[8] + Q;




	/* Measurement */
	kalmangain[0] = H*predictp[0] / (H*H*predictp[0] + R);
	kalmangain[1] = H*predictp[1] / (H*H*predictp[1] + R);
	kalmangain[2] = H*predictp[2] / (H*H*predictp[2] + R);
	kalmangain[3] = H*predictp[3] / (H*H*predictp[3] + R);
	kalmangain[4] = H*predictp[4] / (H*H*predictp[4] + R);
	kalmangain[5] = H*predictp[5] / (H*H*predictp[5] + R);
	kalmangain[6] = H*predictp[6] / (H*H*predictp[6] + R);
	kalmangain[7] = H*predictp[7] / (H*H*predictp[7] + R);
	kalmangain[8] = H*predictp[8] / (H*H*predictp[8] + R);

	predictdata[0] = predictdata[0]
			+ kalmangain[0] * ((float)measuredata[0] - H * predictdata[0]);
	predictdata[1] = predictdata[1]
			+ kalmangain[1] * ((float)measuredata[1] - H * predictdata[1]);
	predictdata[2] = predictdata[2]
			+ kalmangain[2] * ((float)measuredata[2] - H * predictdata[2]);
	predictdata[3] = predictdata[3]
			+ kalmangain[3] * ((float)measuredata[3] - H * predictdata[3]);
	predictdata[4] = predictdata[4]
			+ kalmangain[4] * ((float)measuredata[4] - H * predictdata[4]);
	predictdata[5] = predictdata[5]
			+ kalmangain[5] * ((float)measuredata[5] - H * predictdata[5]);
	predictdata[6] = predictdata[6]
			+ kalmangain[6] * ((float)measuredata[6] - H * predictdata[6]);
	predictdata[7] = predictdata[7]
			+ kalmangain[7] * ((float)measuredata[7] - H * predictdata[7]);
	predictdata[8] = predictdata[8]
			+ kalmangain[8] * ((float)measuredata[8] - H * predictdata[8]);

	predictp[0] = (1 - H*kalmangain[0]) * predictp[0];
	predictp[1] = (1 - H*kalmangain[1]) * predictp[1];
	predictp[2] = (1 - H*kalmangain[2]) * predictp[2];
	predictp[3] = (1 - H*kalmangain[3]) * predictp[3];
	predictp[4] = (1 - H*kalmangain[4]) * predictp[4];
	predictp[5] = (1 - H*kalmangain[5]) * predictp[5];
	predictp[6] = (1 - H*kalmangain[6]) * predictp[6];
	predictp[7] = (1 - H*kalmangain[7]) * predictp[7];
	predictp[8] = (1 - H*kalmangain[8]) * predictp[8];


printdata:
/*
	printf("kalman:	%f	%f	%f\n",
		predictdata[0],
		predictdata[1],
		predictdata[2]
	);
	printf("kalman:	%f	%f	%f\n",
		predictdata[3],
		predictdata[4],
		predictdata[5]
	);
	printf("kalman:	%f	%f	%f\n",
		predictdata[6],
		predictdata[7],
		predictdata[8]
	);
*/
	return 0;
}
