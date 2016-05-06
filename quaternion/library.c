#include<stdio.h>
#include<math.h>




//in:	angle,vectorx,vectory,vectorz
//out:	q0,q1,q2,q3
void axis2quaternion(double* av,double* q)
{
	//normalize vector
	double angle=av[0]*3.141592653/180;
	double norm=sqrt(av[1]*av[1]+av[2]*av[2]+av[3]*av[3]);
	double sinbynorm=sin(angle/2)/norm;

	//create quaternion
	q[0]=cos(angle/2);
	q[1]=av[1]*sinbynorm;
	q[2]=av[2]*sinbynorm;
	q[3]=av[3]*sinbynorm;
}

//in:	q0,q1,q2,q3
//out:	matrix00,01,02,10,11,12,20,21,22
void quaternion2matrix(double* q,double* matrix)
{
	double q0q0=q[0]*q[0];
	double q1q1=q[1]*q[1];
	double q2q2=q[2]*q[2];
	double q3q3=q[3]*q[3];

	matrix[0]=(q0q0+q1q1-q2q2-q3q3);
	matrix[1]=(q[1]*q[2]-q[0]*q[3])*2;
	matrix[2]=(q[0]*q[2]+q[1]*q[3])*2;

	matrix[3]=(q[0]*q[3]+q[1]*q[2])*2;
	matrix[4]=(q0q0-q1q1+q2q2-q3q3);
	matrix[5]=(q[2]*q[3]-q[0]*q[1])*2;

	matrix[6]=(q[1]*q[3]-q[0]*q[2])*2;
	matrix[7]=(q[0]*q[1]+q[2]*q[3])*2;
	matrix[8]=(q0q0-q1q1-q2q2+q3q3);
}

//in:	q0,q1,q2,q3
//out:	pitch,yaw,roll
void quaternion2eulerian(double* q,double* eulerian)
{
	eulerian[0]=atan2( (q[0]*q[1]+q[2]*q[3])*2 , 1-(q[1]*q[1]+q[2]*q[2])*2 );
	eulerian[1]=asin(  (q[0]*q[2]-q[1]*q[3])*2 );
	eulerian[2]=atan2( (q[0]*q[3]+q[1]*q[2])*2 , 1-(q[2]*q[2]+q[3]*q[3])*2 );
}

//in:	q0,q1,q2,q3
//out:	angle,vectorx,vectory,vectorz
void quaternion2axis(double* q,double* av)
{
	double angle=acos(q[0])*2;
	double scale=sqrt(q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);

	av[0]=angle*180/3.141592653;
	av[1]=q[1]/scale;
	av[2]=q[2]/scale;
	av[3]=q[3]/scale;
}

//in:	pitch,yaw,roll
//out:	q0,q1,q2,q3
void eulerian2quaternion(double* eulerian,double* q)
{
	double cospitch=cos(	eulerian[0]/2);
	double cosyaw=cos(	eulerian[1]/2);
	double cosroll=cos(	eulerian[2]/2);
	double sinpitch=sin(	eulerian[0]/2);
	double sinyaw=sin(	eulerian[1]/2);
	double sinroll=sin(	eulerian[2]/2);

	q[0]=cospitch*cosyaw*cosroll + sinpitch*sinyaw*sinroll;
	q[1]=sinpitch*cosyaw*cosroll - cospitch*sinyaw*sinroll;
	q[2]=cospitch*sinyaw*cosroll + sinpitch*cosyaw*sinroll;
	q[3]=cospitch*cosyaw*sinroll - sinpitch*sinyaw*cosroll;
}
