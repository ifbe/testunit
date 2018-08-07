#include<stdio.h>
#include<math.h>
typedef float vec3[3];
typedef float mat3[3][3];
typedef float mat4[4][4];
void mat3_print(mat3 m)
{
	int j;
	for(j=0;j<3;j++)printf("%f,%f,%f\n",m[j][0],m[j][1],m[j][2]);
}
void mat4_print(mat4 m)
{
	int j;
	for(j=0;j<4;j++)printf("%f,%f,%f,%f\n",m[j][0],m[j][1],m[j][2],m[j][3]);
}
float mat3_det(mat3 m)
{
	float t =
	m[0][0]*m[1][1]*m[2][2] +
	m[0][2]*m[1][0]*m[2][1] +
	m[0][1]*m[1][2]*m[2][0] -
	m[0][0]*m[1][2]*m[2][1] -
	m[0][1]*m[1][0]*m[2][2] -
	m[0][2]*m[1][1]*m[2][0];
	mat3_print(m);
	printf("%f\n",t);
	return t;
}
float mat4_det(mat4 m)
{
	mat3 n;
	int j;
	float t = 0.0;

	//m[3][0]
	for(j=0;j<3;j++)
	{
		n[j][0] = m[j][1];
		n[j][1] = m[j][2];
		n[j][2] = m[j][3];
	}
	t -= m[3][0]*mat3_det(n);

	//m[3][1]
	for(j=0;j<3;j++)
	{
		n[j][0] = m[j][0];
		n[j][1] = m[j][2];
		n[j][2] = m[j][3];
	}
	t += m[3][1]*mat3_det(n);

	//m[3][2]
	for(j=0;j<3;j++)
	{
		n[j][0] = m[j][0];
		n[j][1] = m[j][1];
		n[j][2] = m[j][3];
	}
	t -= m[3][2]*mat3_det(n);

	//m[3][3]
	for(j=0;j<3;j++)
	{
		n[j][0] = m[j][0];
		n[j][1] = m[j][1];
		n[j][2] = m[j][2];
	}
	t += m[3][3]*mat3_det(n);

	return t;
}




void main()
{
	int x,y;
	mat4 a,b;
	vec3 t;
	vec3 p0 = {1.0,1.0,0.0};
	vec3 d0 = {1.0,0.0,0.0};
	vec3 p1 = {1.0,1.0,1.0};
	vec3 d1 = {0.0,1.0,0.0};
	t[0] = d0[1]*d1[2] - d0[2]*d1[1];
	t[1] = d0[2]*d1[0] - d0[0]*d1[2];
	t[2] = d0[0]*d1[1] - d0[1]*d1[0];
/*
	for(y=0;y<4;y++)
	{
		for(x=0;x<4;x++)
		{
			a[y][x] = rand()/1000.0;
		}
	}
	a[0][0] = 1.0;
	a[0][1] = 2.0;
	a[0][2] = 3.0;
	a[0][3] = 4.0;
	a[1][0] = 2.0;
	a[1][1] = 3.0;
	a[1][2] = 4.0;
	a[1][3] = 1.0;
	a[2][0] = 3.0;
	a[2][1] = 4.0;
	a[2][2] = 1.0;
	a[2][3] = 2.0;
	a[3][0] = 4.0;
	a[3][1] = 1.0;
	a[3][2] = 2.0;
	a[3][3] = 3.0;
	mat4_print(a);
	printf("%f\n",mat4_det(a));
*/
	a[0][0] = p1[0] - p0[0];
	a[1][0] = p1[1] - p0[1];
	a[2][0] = p1[2] - p0[2];
	a[0][1] = d1[0];
	a[1][1] = d1[1];
	a[2][1] = d1[2];
	a[0][2] = t[0];
	a[1][2] = t[1];
	a[2][2] = t[2];
	mat4_print(a);
	printf("%f\n",mat4_det(a));

	b[0][0] = p1[0] - p0[0];
	b[1][0] = p1[1] - p0[1];
	b[2][0] = p1[2] - p0[2];
	b[0][1] = d0[0];
	b[1][1] = d0[1];
	b[2][1] = d0[2];
	b[0][2] = t[0];
	b[1][2] = t[1];
	b[2][2] = t[2];
	mat4_print(b);
	printf("%f\n",mat4_det(b));
}