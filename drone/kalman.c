#include <stdio.h>
#include <string.h>




//input
extern	short measuredata[6];
	float kalmangain[6];
	float H;
	float R;

//output
	float predictdata[6];
	float predictp[6];
	float A;
	float Q;




int initkalman()
{
	predictdata[0] = predictdata[1] = predictdata[2] = predictdata[3] = predictdata[4] =predictdata[5] = 42;
	predictp[0] = predictp[1] = predictp[2] = predictp[3] = predictp[4] = predictp[5] = 1;
	A = 1;			//testonly=1
	H = 1;
	Q = 1;		//10e-3;	/* predict noise convariance */
	R = 50;	//10e-2;	/* measure error convariance */
}
void killkalman()
{
}
int kalman()
{
	/* Predict		p(n|n-1)=A^2*p(n-1|n-1)+q */
	predictdata[0] = A * predictdata[0];
	predictdata[1] = A * predictdata[1];
	predictdata[2] = A * predictdata[2];
	predictdata[3] = A * predictdata[3];
	predictdata[4] = A * predictdata[4];
	predictdata[5] = A * predictdata[5];

	predictp[0] = A*A*predictp[0] + Q;
	predictp[1] = A*A*predictp[1] + Q;
	predictp[2] = A*A*predictp[2] + Q;
	predictp[3] = A*A*predictp[3] + Q;
	predictp[4] = A*A*predictp[4] + Q;
	predictp[5] = A*A*predictp[5] + Q;




	/* Measurement */
	kalmangain[0] = H*predictp[0] / (H*H*predictp[0] + R);
	kalmangain[1] = H*predictp[1] / (H*H*predictp[1] + R);
	kalmangain[2] = H*predictp[2] / (H*H*predictp[2] + R);
	kalmangain[3] = H*predictp[3] / (H*H*predictp[3] + R);
	kalmangain[4] = H*predictp[4] / (H*H*predictp[4] + R);
	kalmangain[5] = H*predictp[5] / (H*H*predictp[5] + R);

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

	predictp[0] = (1 - H*kalmangain[0]) * predictp[0];
	predictp[1] = (1 - H*kalmangain[1]) * predictp[1];
	predictp[2] = (1 - H*kalmangain[2]) * predictp[2];
	predictp[3] = (1 - H*kalmangain[3]) * predictp[3];
	predictp[4] = (1 - H*kalmangain[4]) * predictp[4];
	predictp[5] = (1 - H*kalmangain[5]) * predictp[5];
/*
	printf("(%f,%f,%f),(%f,%f,%f)\n",
		predictdata[0],
		predictdata[1],
		predictdata[2],
		predictdata[3],
		predictdata[4],
		predictdata[5]
	);
*/
}
