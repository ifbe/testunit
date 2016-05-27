#include<stdio.h>

//in
extern float imu[3];
extern float mahony[2];

//out
float eulerian[3];




void state()
{
	eulerian[0] = ( imu[0] + mahony[0] ) / 2;
	eulerian[1] = mahony[1];
	eulerian[2] = ( imu[2] + mahony[2] ) / 2;
/*
	printf("%f	%f	%f\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);
*/
}
