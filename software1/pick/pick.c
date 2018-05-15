#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define PI 3.1415926535897932384626433832795028841971693993151
typedef float vec4[4];
typedef float mat4[4][4];




void fixview(mat4 viewmatrix)
{
	//a X b = [ay*bz - az*by, az*bx-ax*bz, ax*by-ay*bx]
	float norm;
	float cx = 100.0;
	float cy = -100.0;
	float cz = 100.0;

	//uvn.n = front
	float nx = 0.0-cx;
	float ny = 0.0-cy;
	float nz = 0.0-cz;
	norm = sqrt(nx*nx + ny*ny + nz*nz);
	nx /= norm;
	ny /= norm;
	nz /= norm;

	//uvn.u = right = cross(front,(0,0,1))
	float ux = ny*1 - nz*0;
	float uy = nz*0 - nx*1;
	float uz = nx*0 - ny*0;
	norm = sqrt(ux*ux + uy*uy + uz*uz);
	ux /= norm;
	uy /= norm;
	uz /= norm;

	//uvn.v = above cross(right, front)
	float vx = uy*nz - uz*ny;
	float vy = uz*nx - ux*nz;
	float vz = ux*ny - uy*nx;
	norm = sqrt(vx*vx + vy*vy + vz*vz);
	vx /= norm;
	vy /= norm;
	vz /= norm;

	viewmatrix[0][0] = ux;
	viewmatrix[0][1] = uy;
	viewmatrix[0][2] = uz;
	viewmatrix[0][3] = -cx*ux - cy*uy - cz*uz;

	viewmatrix[1][0] = vx;
	viewmatrix[1][1] = vy;
	viewmatrix[1][2] = vz;
	viewmatrix[1][3] = -cx*vx - cy*vy - cz*vz;

	viewmatrix[2][0] = -nx;
	viewmatrix[2][1] = -ny;
	viewmatrix[2][2] = -nz;
	viewmatrix[2][3] = cx*nx + cy*ny + cz*nz;

	viewmatrix[3][0] = 0.0f;
	viewmatrix[3][1] = 0.0f;
	viewmatrix[3][2] = 0.0f;
	viewmatrix[3][3] = 1.0f;
}
void fixproj(mat4 projmatrix)
{
	float a = PI/2;
	float n = 1.0;
	float w = 512.0;
	float h = 512.0;

	projmatrix[0][0] = 1.0 / tan(a/2);
	projmatrix[0][1] = 0.0;
	projmatrix[0][2] = 0.0;
	projmatrix[0][3] = 0.0;

	projmatrix[1][0] = 0.0;
	projmatrix[1][1] = projmatrix[0][0] * w / h;
	projmatrix[1][2] = 0.0;
	projmatrix[1][3] = 0.0;

	projmatrix[2][0] = 0.0;
	projmatrix[2][1] = 0.0;
	projmatrix[2][2] = -1.0;	//	(n+f) / (n-f);
	projmatrix[2][3] = -2*n;	//	2*n*f / (n-f);

	projmatrix[3][0] = 0.0;
	projmatrix[3][1] = 0.0;
	projmatrix[3][2] = -1.0;
	projmatrix[3][3] = 0.0;
}
void invproj(mat4 projmatrix)
{
	float a = PI/2;
	float n = 1.0;
	float w = 512.0;
	float h = 512.0;

	projmatrix[0][0] = tan(a/2);
	projmatrix[0][1] = 0.0;
	projmatrix[0][2] = 0.0;
	projmatrix[0][3] = 0.0;

	projmatrix[1][0] = 0.0;
	projmatrix[1][1] = projmatrix[0][0] * h / w;
	projmatrix[1][2] = 0.0;
	projmatrix[1][3] = 0.0;

	projmatrix[2][0] = 0.0;
	projmatrix[2][1] = 0.0;
	projmatrix[2][2] = 0.0;
	projmatrix[2][3] = -1.0;

	projmatrix[3][0] = 0.0;
	projmatrix[3][1] = 0.0;
	projmatrix[3][2] = -0.5/n;
	projmatrix[3][3] = 0.5/n;
}
void invview(mat4 viewmatrix)
{
	//a X b = [ay*bz - az*by, az*bx-ax*bz, ax*by-ay*bx]
	float norm;
	float cx = 100.0;
	float cy = -100.0;
	float cz = 100.0;

	//uvn.n = front
	float nx = 0.0-cx;
	float ny = 0.0-cy;
	float nz = 0.0-cz;
	norm = sqrt(nx*nx + ny*ny + nz*nz);
	nx /= norm;
	ny /= norm;
	nz /= norm;

	//uvn.u = right = cross(front,(0,0,1))
	float ux = ny*1 - nz*0;
	float uy = nz*0 - nx*1;
	float uz = nx*0 - ny*0;
	norm = sqrt(ux*ux + uy*uy + uz*uz);
	ux /= norm;
	uy /= norm;
	uz /= norm;

	//uvn.v = above cross(right, front)
	float vx = uy*nz - uz*ny;
	float vy = uz*nx - ux*nz;
	float vz = ux*ny - uy*nx;
	norm = sqrt(vx*vx + vy*vy + vz*vz);
	vx /= norm;
	vy /= norm;
	vz /= norm;

	viewmatrix[0][0] = ux;
	viewmatrix[0][1] = vx;
	viewmatrix[0][2] = -nx;
	viewmatrix[0][3] = cx;

	viewmatrix[1][0] = uy;
	viewmatrix[1][1] = vy;
	viewmatrix[1][2] = -ny;
	viewmatrix[1][3] = cy;

	viewmatrix[2][0] = uz;
	viewmatrix[2][1] = vz;
	viewmatrix[2][2] = -nz;
	viewmatrix[2][3] = cz;

	viewmatrix[3][0] = 0.0f;
	viewmatrix[3][1] = 0.0f;
	viewmatrix[3][2] = 0.0f;
	viewmatrix[3][3] = 1.0f;
}
void check(vec4 v)
{
	vec4 p = {v[0],v[1],v[2],v[3]};
	vec4 q;
	mat4 m;
	invproj(m);
	q[3] = m[3][0]*p[0]+m[3][1]*p[1]+m[3][2]*p[2]+m[3][3]*p[3];
	q[0] = (m[0][0]*p[0]+m[0][1]*p[1]+m[0][2]*p[2]+m[0][3]*p[3])/q[3];
	q[1] = (m[1][0]*p[0]+m[1][1]*p[1]+m[1][2]*p[2]+m[1][3]*p[3])/q[3];
	q[2] = (m[2][0]*p[0]+m[2][1]*p[1]+m[2][2]*p[2]+m[2][3]*p[3])/q[3];
	q[3] = 1.0;
	printf("%f,%f,%f,%f\n",q[0],q[1],q[2],q[3]);

	invview(m);
	p[0] = m[0][0]*q[0]+m[0][1]*q[1]+m[0][2]*q[2]+m[0][3]*q[3];
	p[1] = m[1][0]*q[0]+m[1][1]*q[1]+m[1][2]*q[2]+m[1][3]*q[3];
	p[2] = m[2][0]*q[0]+m[2][1]*q[1]+m[2][2]*q[2]+m[2][3]*q[3];
	p[3] = m[3][0]*q[0]+m[3][1]*q[1]+m[3][2]*q[2]+m[3][3]*q[3];
	printf("%f,%f,%f,%f\n",p[0],p[1],p[2],p[3]);

	fixview(m);
	q[0] = m[0][0]*p[0]+m[0][1]*p[1]+m[0][2]*p[2]+m[0][3]*p[3];
	q[1] = m[1][0]*p[0]+m[1][1]*p[1]+m[1][2]*p[2]+m[1][3]*p[3];
	q[2] = m[2][0]*p[0]+m[2][1]*p[1]+m[2][2]*p[2]+m[2][3]*p[3];
	q[3] = m[3][0]*p[0]+m[3][1]*p[1]+m[3][2]*p[2]+m[3][3]*p[3];
	printf("%f,%f,%f,%f\n",q[0],q[1],q[2],q[3]);

	fixproj(m);
	p[3] = m[3][0]*q[0]+m[3][1]*q[1]+m[3][2]*q[2]+m[3][3]*q[3];
	p[0] = (m[0][0]*q[0]+m[0][1]*q[1]+m[0][2]*q[2]+m[0][3]*q[3])/p[3];
	p[1] = (m[1][0]*q[0]+m[1][1]*q[1]+m[1][2]*q[2]+m[1][3]*q[3])/p[3];
	p[2] = (m[2][0]*q[0]+m[2][1]*q[1]+m[2][2]*q[2]+m[2][3]*q[3])/p[3];
	p[3] = 1.0;
	printf("%f,%f,%f,%f\n",p[0],p[1],p[2],p[3]);
}
void main()
{
	float t;
	vec4 v = {0.0,0.0,0.0,1.0};

	for(t=-1.0;t<2.0;t+=1.0)
	{
		v[2] = t;

		v[0] = -1.0;
		v[1] = -1.0;
		printf("\n%f,%f,%f,%f\n",v[0],v[1],v[2],v[3]);
		check(v);

		v[0] = 1.0;
		v[1] = -1.0;
		printf("\n%f,%f,%f,%f\n",v[0],v[1],v[2],v[3]);
		check(v);

		v[0] = -1.0;
		v[1] = 1.0;
		printf("\n%f,%f,%f,%f\n",v[0],v[1],v[2],v[3]);
		check(v);

		v[0] = 1.0;
		v[1] = 1.0;
		printf("\n%f,%f,%f,%f\n",v[0],v[1],v[2],v[3]);
		check(v);
	}
}