#include<stdio.h>

//pitch0,yaw0,roll0......pitch1,yaw1,roll1.....pitch2,yaw2,roll2......
extern float eulerian[9];

//longitude0,latitude0,height0......longitude1,latitude1,height1......
extern float drift[9];




void state()
{
	//use 345,678,9ab,cde......to fix 012?
	//eulerian[0] = ( imu[0] + mahony[0] ) / 2;
	//eulerian[1] = mahony[1];
	//eulerian[2] = ( imu[2] + mahony[2] ) / 2;
	//drift[0]=
	//drift[1]=
	//drift[2]=


	printf("1:	%f	%f	%f\n",
		eulerian[0],
		eulerian[1],
		eulerian[2]
	);

	printf("2:	%f	%f	%f\n",
		eulerian[3],
		eulerian[4],
		eulerian[5]
	);

}
