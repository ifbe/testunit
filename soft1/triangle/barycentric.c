#include<stdio.h>


void barycentric(float* o, float* p, float* a, float* b, float* c)
{
	o[0] = (b[1]-c[1])*p[0]
	     + (c[0]-b[0])*p[1]
	     + (b[0]*c[1] - c[0]*b[1]);
	o[0]/= (b[1] - c[1])*a[0]
	     + (c[0] - b[0])*a[1]
	     + (b[0]*c[1] - c[0]*b[1]);

	o[1] = (a[1] - c[1]) * p[0]
	     + (c[0] - a[0]) * p[1]
	     + (a[0]*c[1] - c[0]*a[1]);
	o[1]/= (a[1] - c[1]) * b[0]
	     + (c[0] - a[0]) * b[1]
	     + (a[0]*c[1] - c[0]*a[1]);

	o[2] = 1.0 - o[0] - o[1];
}
void barycentric2(float* o, float* p, float* a, float* b, float* c)
{
	float jx = b[0] - a[0];
	float jy = b[1] - a[1];
	float kx = c[0] - a[0];
	float ky = c[1] - a[1];
	float px = p[0] - a[0];
	float py = p[1] - a[1];

	float jj = jx*jx + jy*jy;
	float jk = jx*kx + jy*ky;
	float jp = jx*px + jy*py;
	float kk = kx*kx + ky*ky;
	float kp = kx*px + ky*py;

	float inv = 1.0 / (jj*kk - jk*jk);
	o[1] = (kk*jp - jk*kp) * inv;
	o[2] = (jj*kp - jk*jp) * inv;
	o[0] = 1.0 - o[1] - o[2];
}
int main()
{
	float o[3];
	float p[2] = {1.0,1.0};
	float a[2] = {0.0,0.0};
	float b[2] = {3.0,0.0};
	float c[2] = {0.0,4.0};

	barycentric(o,p,a,b,c);
	printf("%f,%f,%f\n",o[0],o[1],o[2]);

	barycentric2(o,p,a,b,c);
	printf("%f,%f,%f\n",o[0],o[1],o[2]);
	return 0;
}
