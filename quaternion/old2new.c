#include<stdio.h>
#include<math.h>
void axis2quaternion(double*,double*);
void quaternion2matrix(double*,double*);




//easy understand:	angle,vectorx,vectory,vectorz
double axis[4];
//eulerian angle:	pitch,yaw,roll
double eulerian[3];
//quaternion:		cos,x*sin,y*sin,z*sin
double quaternion[4];
//rotation matrix:
double matrix[3][3];




//in
double xin,yin,zin;
//out
double xout,yout,zout;




void main()
{
	//input symmetry axis
	printf("(angle,vx,vy,vz)=");
	scanf("%lf,%lf,%lf,%lf",
		&axis[0],
		&axis[1],
		&axis[2],
		&axis[3]
	);

	//vector->quaternion
	axis2quaternion(axis,quaternion);
	printf("quaternion=\n");
	printf("	(%lf,%lf,%lf,%lf)\n\n",
		quaternion[0],
		quaternion[1],
		quaternion[2],
		quaternion[3]
	);

	//quaternion->matrix
	quaternion2matrix(quaternion,&matrix[0][0]);
	printf("matrix=\n");
	printf("	[%lf %lf %lf]\n",matrix[0][0],matrix[0][1],matrix[0][2]);
	printf("	[%lf %lf %lf]\n",matrix[1][0],matrix[1][1],matrix[1][2]);
	printf("	[%lf %lf %lf]\n\n",matrix[2][0],matrix[2][1],matrix[2][2]);

	//input vector
	printf("(xin,yin,zin)=");
	scanf("%lf,%lf,%lf",&xin,&yin,&zin);

	//old->new
	xout=matrix[0][0]*xin + matrix[0][1]*yin + matrix[0][2]*zin;
	yout=matrix[1][0]*xin + matrix[1][1]*yin + matrix[1][2]*zin;
	zout=matrix[2][0]*xin + matrix[2][1]*yin + matrix[2][2]*zin;
	printf("newvector=(%lf,%lf,%lf)\n",xout,yout,zout);
}
