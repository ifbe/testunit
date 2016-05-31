#include<stdio.h>

//pitch0,yaw0,roll0......pitch1,yaw1,roll1.....pitch2,yaw2,roll2......
float eulerian[9];

//longitude0,latitude0,height0......longitude1,latitude1,height1......
float earth[9];




void state()
{
	//use 345,678,9ab,cde......to fix 012?
	//eulerian[0] = ( imu[0] + mahony[0] ) / 2;
	//eulerian[1] = mahony[1];
	//eulerian[2] = ( imu[2] + mahony[2] ) / 2;
	//earth[0]=
	//earth[1]=
	//earth[2]=
/*
	printf("%f	%f	%f	%f	%f	%f\n",
		eulerian[0],
		eulerian[1],
		eulerian[2],
		eulerian[3],
		eulerian[4],
		eulerian[5]
	);
*/
}
