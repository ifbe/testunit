#include <stdio.h>
#include <math.h>
void axisangle4matrix(float* a, float (*m)[3])
{
	float n;
	a[0] = m[2][1] - m[1][2];
	a[1] = m[0][2] - m[2][0];
	a[2] = m[1][0] - m[0][1];

	n = acos((m[0][0]+m[1][1]+m[2][2]-1.0) / 2.0);
	n/= sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	a[0] *= n;
	a[1] *= n;
	a[2] *= n;
}
int main()
{
	float a[3];
	float m[3][3] = {{0.866,-0.5, 0.0},{0.5, 0.866, 0.0},{0.0, 0.0, 1.0}};
	axisangle4matrix(a,m);
	printf("%f,%f,%f\n", a[0],a[1],a[2]);
	return 0;
}
