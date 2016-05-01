#include<stdio.h>
#include<math.h>




//rotation
	//angle and vector
	double angle,vx,vy,vz;

	//eulerian angle
	double alpha,beta,mygamma;

	//quaternion
	double q0,q1,q2,q3;

	//rotation matrix
	double matrix[3][3];




//the vector we want
	//in
	double xin,yin,zin;

	//out
	double xout,yout,zout;




//
void vector2quaternion()
{
	//normalize vector
	double temp=sqrt(vx*vx+vy*vy+vz*vz);
	angle=angle*3.141592653/180;

	//create quaternion
	q0=cos(angle/2);
	q1=sin(angle/2)*vx/temp;
	q2=sin(angle/2)*vy/temp;
	q3=sin(angle/2)*vz/temp;
}

void quaternion2matrix()
{
	matrix[0][0]=(q0*q0+q1*q1-q2*q2-q3*q3);
	matrix[0][1]=(q1*q2-q0*q3)*2;
	matrix[0][2]=(q0*q2+q1*q3)*2;

	matrix[1][0]=(q0*q3+q1*q2)*2;
	matrix[1][1]=(q0*q0-q1*q1+q2*q2-q3*q3);
	matrix[1][2]=(q2*q3-q0*q1)*2;

	matrix[2][0]=(q1*q3-q0*q2)*2;
	matrix[2][1]=(q0*q1+q2*q3)*2;
	matrix[2][2]=(q0*q0-q1*q1-q2*q2+q3*q3);
}

void quaternion2eulerian()
{
	alpha=atan2( (q0*q1+q2*q3)*2 , 1-(q1*q1+q2*q2)*2 );
	beta=asin((q0*q2-q1*q3)*2);
	mygamma=atan2( (q0*q3+q1*q2)*2 , 1-(q2*q2+q3*q3)*2 );
}

void eulerian2quaternion()
{
	double cos1=cos(alpha/2);
	double cos2=cos(beta/2);
	double cos3=cos(mygamma/2);
	double sin1=sin(alpha/2);
	double sin2=sin(beta/2);
	double sin3=sin(mygamma/2);

	q0=cos1*cos2*cos3 + sin1*sin2*sin3;
	q1=sin1*cos2*cos3 - cos1*sin2*sin3;
	q2=cos1*sin2*cos3 + sin1*cos2*sin3;
	q3=cos1*cos2*sin3 - sin1*sin2*cos3;
}

void oldvector2newvector()
{
	xout=matrix[0][0]*xin + matrix[0][1]*yin + matrix[0][2]*zin;
	yout=matrix[1][0]*xin + matrix[1][1]*yin + matrix[1][2]*zin;
	zout=matrix[2][0]*xin + matrix[2][1]*yin + matrix[2][2]*zin;
}

void main()
{
	//input symmetry axis
	printf("(angle,vx,vy,vz)=");
	scanf("%lf,%lf,%lf,%lf",&angle,&vx,&vy,&vz);

	//vector->quaternion
	vector2quaternion();
	printf("quaternion=\n");
	printf("	(%lf,%lf,%lf,%lf)\n\n",q0,q1,q2,q3);

	//quaternion->matrix
	quaternion2matrix();
	printf("matrix=\n");
	printf("	[%lf %lf %lf]\n",matrix[0][0],matrix[0][1],matrix[0][2]);
	printf("	[%lf %lf %lf]\n",matrix[1][0],matrix[1][1],matrix[1][2]);
	printf("	[%lf %lf %lf]\n\n",matrix[2][0],matrix[2][1],matrix[2][2]);

	//input vector
	printf("(xin,yin,zin)=");
	scanf("%lf,%lf,%lf",&xin,&yin,&zin);

	//old->new
	oldvector2newvector();
	printf("newvector=(%lf,%lf,%lf)\n",xout,yout,zout);
}