#include<stdio.h>
#include<math.h>
void quaternion2matrix(double*,double*);
void quaternion2eulerian(double*,double*);
void quaternion2axis(double*,double*);




//easy understand:	angle,vectorx,vectory,vectorz
double axis[4];
//eulerian angle:	pitch,yaw,roll
double eulerian[3];
//quaternion:		cos,x*sin,y*sin,z*sin
double quaternion[4];
//rotation matrix:
double matrix[3][3];




void main()
{
	//input symmetry axis
	printf("(q0,q1,q2,q3)=");
	scanf("%lf,%lf,%lf,%lf",
		&quaternion[0],
		&quaternion[1],
		&quaternion[2],
		&quaternion[3]
	);

	//quaternion->matrix
	quaternion2matrix(quaternion,&matrix[0][0]);
	printf("matrix=\n");
	printf("	[%lf %lf %lf]\n",matrix[0][0],matrix[0][1],matrix[0][2]);
	printf("	[%lf %lf %lf]\n",matrix[1][0],matrix[1][1],matrix[1][2]);
	printf("	[%lf %lf %lf]\n\n",matrix[2][0],matrix[2][1],matrix[2][2]);

	//quaternion->eulerian
	quaternion2eulerian(quaternion,eulerian);
	printf("pitch,yaw,roll=\n");
	printf("	%lf,%lf,%lf\n\n",eulerian[0],eulerian[1],eulerian[2]);

	//quaternion->axis
	quaternion2axis(quaternion,axis);
	printf("angle,vectorx,vectory,vectorz=\n");
	printf("	%lf,%lf,%lf,%lf\n\n",axis[0],axis[1],axis[2],axis[3]);
}
