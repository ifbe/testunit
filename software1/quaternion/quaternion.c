#include<stdio.h>
#include<math.h>
#define PI 3.14159265358979323846264338327950




//in:	vx,vy,vz,angle
//out:	qx,qy,qz,qw
void axis2quaternion(float* av, float* q)
{
	//normalize vector
	float angle = av[3]*PI/360.0;
	float norm = sqrt(av[0]*av[0] + av[1]*av[1] + av[2]*av[2]);
	float sinbynorm = sin(angle)/norm;

	//create quaternion
	q[0] = av[0]*sinbynorm;
	q[1] = av[1]*sinbynorm;
	q[2] = av[2]*sinbynorm;
	q[3] = cos(angle);
}
//in:	pitch,yaw,roll
//out:	qx,qy,qz,qw
void eulerian2quaternion(float* eulerian, float* q)
{
	float pitch = eulerian[0]*PI/360.0;
	float yaw = eulerian[1]*PI/360.0;
	float roll = eulerian[2]*PI/360.0;
	float cospitch = cos(pitch);
	float sinpitch = sin(pitch);
	float cosyaw = cos(yaw);
	float sinyaw = sin(yaw);
	float cosroll = cos(roll);
	float sinroll = sin(roll);

	q[0] = sinpitch*cosyaw*cosroll - cospitch*sinyaw*sinroll;
	q[1] = cospitch*sinyaw*cosroll + sinpitch*cosyaw*sinroll;
	q[2] = cospitch*cosyaw*sinroll - sinpitch*sinyaw*cosroll;
	q[3] = cospitch*cosyaw*cosroll + sinpitch*sinyaw*sinroll;
}
//in:	matrix
//out:	qx,qy,qz,qw
void matrix2quaternion(float* matrix, float* q)
{
}




//in:	qx,qy,qz,qw
//out:	vx,vy,vz,angle
void quaternion2axis(float* q, float* av)
{
	float angle = acos(q[3]);
	float scale = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);

	av[0] = q[0]/scale;
	av[1] = q[1]/scale;
	av[2] = q[2]/scale;
	av[3] = angle*360/3.141592653;
}
//in:	qx,qy,qz,qw
//out:	pitch,yaw,roll
void quaternion2eulerian(float* q, float* eulerian)
{
	eulerian[0] = atan2( (q[3]*q[1]+q[2]*q[0])*2 , 1-(q[1]*q[1]+q[2]*q[2])*2 );
	eulerian[0] *= 180.0/PI;
	eulerian[1] = atan2( (q[3]*q[0]+q[1]*q[2])*2 , 1-(q[2]*q[2]+q[0]*q[0])*2 );
	eulerian[1] *= 180.0/PI;
	eulerian[2] = asin(  (q[3]*q[2]-q[1]*q[0])*2 );
	eulerian[2] *= 180.0/PI;
}
//in:	qx,qy,qz,qw
//out:	matrix
void quaternion2matrix(float* q, float* matrix)
{
	matrix[0] = (q[3]*q[3] + q[1]*q[1] - q[2]*q[2] - q[0]*q[0]);
	matrix[1] = (q[1]*q[2] - q[3]*q[0])*2;
	matrix[2] = (q[3]*q[2] + q[1]*q[0])*2;

	matrix[3] = (q[3]*q[0] + q[1]*q[2])*2;
	matrix[4] = (q[3]*q[3] - q[1]*q[1] + q[2]*q[2] - q[0]*q[0]);
	matrix[5] = (q[2]*q[0] - q[3]*q[1])*2;

	matrix[6] = (q[1]*q[0] - q[3]*q[2])*2;
	matrix[7] = (q[3]*q[1] + q[2]*q[0])*2;
	matrix[8] = (q[3]*q[3] - q[1]*q[1] - q[2]*q[2] + q[0]*q[3]);
}




void quaternionnormalize(float* p)
{
	float norm = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2] + p[3]*p[3]);
	p[0] /= norm;
	p[1] /= norm;
	p[2] /= norm;
	p[3] /= norm;
}
//in:	left, right
//out:	left = left*right
void quaternionmultiply(float* l, float* r)
{
	float t[4];
	t[0] = l[0];
	t[1] = l[1];
	t[2] = l[2];
	t[3] = l[3];

	l[0] = t[3]*r[0] + t[0]*r[3] + t[1]*r[2] - t[2]*r[1];
	l[1] = t[3]*r[1] - t[0]*r[2] + t[1]*r[3] + t[2]*r[0];
	l[2] = t[3]*r[2] + t[0]*r[1] - t[1]*r[0] + t[2]*r[3];
	l[3] = t[3]*r[3] - t[0]*r[0] - t[1]*r[1] - t[2]*r[2];
}
//in:	vector, quaternion
//out:	vector
void quaternionrotate(float* v, float* q)
{
	//t = 2 * cross(q.xyz, v)
	//v' = v + q.w * t + cross(q.xyz, t)
	//float tx = 2*q[2]*v[2]
	float j[3];
	float k[3];
	j[0] = (q[1]*v[2]-q[2]*v[1]) * 2;
	j[1] = (q[2]*v[0]-q[0]*v[2]) * 2;
	j[2] = (q[0]*v[1]-q[1]*v[0]) * 2;

	k[0] = q[1]*j[2]-q[2]*j[1];
	k[1] = q[2]*j[0]-q[0]*j[2];
	k[2] = q[0]*j[1]-q[1]*j[0];

	v[0] += q[3]*j[0] + k[0];
	v[1] += q[3]*j[1] + k[1];
	v[2] += q[3]*j[2] + k[2];
}




void main()
{
	float iaxis[4] = {0.0, 0.0, 1.0, 60.0};
	float ieular[3] = {0.0, 0.0, 60.0};
	float imatrix[9];

	float oaxis[4];
	float oeular[3];
	float omatrix[9];

	float quaternion[4];
	float vector[4] = {0.866025, -0.5, 3.0, 0.0};
	float temp[4];
	float conj[4];




	//
	printf("axis2quaternion:\n");
	axis2quaternion(iaxis, quaternion);
	printf("	%lf, %lf, %lf, %lf\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3]);

	//
	printf("eulerian2quaternion:\n");
	eulerian2quaternion(ieular, quaternion);
	printf("	%lf, %lf, %lf, %lf\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3]);

	//
	printf("matrix2quaternion:\n");
	matrix2quaternion(imatrix, quaternion);
	printf("	%lf, %lf, %lf, %lf\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3]);




	//
	printf("quaternion2axis:\n");
	quaternion2axis(quaternion, oaxis);
	printf("	%lf, %lf, %lf, %lf\n", oaxis[0], oaxis[1], oaxis[2], oaxis[3]);

	//
	printf("quaternion2eulerian:\n");
	quaternion2eulerian(quaternion, oeular);
	printf("	%lf, %lf, %lf\n", oeular[0], oeular[1], oeular[2]);

	//
	printf("quaternion2matrix:\n");
	quaternion2matrix(quaternion, omatrix);
	printf(
		"	%lf, %lf, %lf\n"
		"	%lf, %lf, %lf\n"
		"	%lf, %lf, %lf\n",
		omatrix[0], omatrix[1], omatrix[2],
		omatrix[3], omatrix[4], omatrix[5],
		omatrix[6], omatrix[7], omatrix[8]
	);




	//
	printf("quaternionmultiply:\n");
	temp[0]=quaternion[0];temp[1]=quaternion[1];temp[2]=quaternion[2];temp[3]=quaternion[3];
	conj[0]=-quaternion[0];conj[1]=-quaternion[1];conj[2]=-quaternion[2];conj[3]=quaternion[3];
	quaternionmultiply(temp, vector);
	printf("	%lf, %lf, %lf, %lf\n", temp[0], temp[1], temp[2], temp[3]);
	quaternionmultiply(temp, conj);
	printf("	%lf, %lf, %lf, %lf\n", temp[0], temp[1], temp[2], temp[3]);

	//
	printf("quaternionrotate:\n");
	quaternionrotate(vector, quaternion);
	printf("	%lf, %lf, %lf\n", vector[0], vector[1], vector[2]);
}