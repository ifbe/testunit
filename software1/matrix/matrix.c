#include<stdio.h>




void matrixtranspose_2(float* u)
{
	float t;
	t = u[1];
	u[1] = u[2];
	u[2] = t;
}
void matrixtranspose_3(float* u)
{
	float t;

	t = u[1];
	u[1] = u[3];
	u[3] = t;

	t = u[2];
	u[2] = u[6];
	u[6] = t;

	t = u[5];
	u[5] = u[7];
	u[7] = t;
}
void matrixtranspose_4(float* u)
{
	float t;

	t = u[1];
	u[1] = u[4];
	u[4] = t;

	t = u[2];
	u[2] = u[8];
	u[8] = t;

	t = u[3];
	u[3] = u[12];
	u[12] = u[3];

	t = u[6];
	u[6] = u[9];
	u[9] = t;

	t = u[7];
	u[7] = u[13];
	u[13] = t;

	t = u[11];
	u[11] = u[14];
	u[14] = t;
}




int matrixinverse_2(float* d, float* s)
{
	float t = s[0]*s[3] - s[1]*s[2];
	if((t > -1e-50)&&(t < 1e-50))return -1;

	d[0] = s[3] / t;
	d[1] = -s[1] / t;
	d[2] = -s[2] / t;
	d[3] = s[0] / t;
	return 0;
}
int matrixinverse_3(float* d, float* s)
{
	float t = s[0]*(s[4]*s[8]-s[5]*s[7])
			- s[3]*(s[1]*s[8]-s[2]*s[7])
			+ s[6]*(s[1]*s[5]-s[2]*s[4]);
	if((t > -1e-50)&&(t < 1e-50))return -1;
	d[0] = (s[4]*s[8] - s[5]*s[7])/t;
	d[1] = (s[2]*s[7] - s[1]*s[8])/t;
	d[2] = (s[1]*s[5] - s[2]*s[4])/t;
	d[3] = (s[5]*s[6] - s[3]*s[8])/t;
	d[4] = (s[0]*s[8] - s[2]*s[6])/t;
	d[5] = (s[3]*s[2] - s[0]*s[5])/t;
	d[6] = (s[3]*s[7] - s[4]*s[6])/t;
	d[7] = (s[1]*s[6] - s[0]*s[7])/t;
	d[8] = (s[0]*s[4] - s[3]*s[1])/t;

	return 0;
}
int matrixinverse_4(float* d, float* s)
{
}




void matrixmultiply_2(float* u, float* v)
{
	int j;
	float w[4];
	for(j=0;j<4;j++)w[j] = u[j];

	u[0] = w[0]*v[0] + w[1]*v[2];
	u[1] = w[0]*v[1] + w[1]*v[3];

	u[2] = w[2]*v[0] + w[3]*v[2];
	u[3] = w[2]*v[1] + w[3]*v[3];
}
void matrixmultiply_3(float* u, float* v)
{
	int j;
	float w[9];
	for(j=0;j<9;j++)w[j] = u[j];

	u[0] = w[0]*v[0] + w[1]*v[3] + w[2]*v[6];
	u[1] = w[0]*v[1] + w[1]*v[4] + w[2]*v[7];
	u[2] = w[0]*v[2] + w[1]*v[5] + w[2]*v[8];

	u[3] = w[3]*v[0] + w[4]*v[3] + w[5]*v[6];
	u[4] = w[3]*v[1] + w[4]*v[4] + w[5]*v[7];
	u[5] = w[3]*v[2] + w[4]*v[5] + w[5]*v[8];

	u[6] = w[6]*v[0] + w[7]*v[3] + w[8]*v[6];
	u[7] = w[6]*v[1] + w[7]*v[4] + w[8]*v[7];
	u[8] = w[6]*v[2] + w[7]*v[5] + w[8]*v[8];
}
void matrixmultiply(float* u, float* v)
{
	int j;
	float w[16];
	for(j=0;j<16;j++)w[j] = u[j];

	u[ 0] = w[ 0]*v[ 0] + w[ 1]*v[ 4] + w[ 2]*v[ 8] + w[ 3]*v[12];
	u[ 1] = w[ 0]*v[ 1] + w[ 1]*v[ 5] + w[ 2]*v[ 9] + w[ 3]*v[13];
	u[ 2] = w[ 0]*v[ 2] + w[ 1]*v[ 6] + w[ 2]*v[10] + w[ 3]*v[14];
	u[ 3] = w[ 0]*v[ 3] + w[ 1]*v[ 7] + w[ 2]*v[11] + w[ 3]*v[15];

	u[ 4] = w[ 4]*v[ 0] + w[ 5]*v[ 4] + w[ 6]*v[ 8] + w[ 7]*v[12];
	u[ 5] = w[ 4]*v[ 1] + w[ 5]*v[ 5] + w[ 6]*v[ 9] + w[ 7]*v[13];
	u[ 6] = w[ 4]*v[ 2] + w[ 5]*v[ 6] + w[ 6]*v[10] + w[ 7]*v[14];
	u[ 7] = w[ 4]*v[ 3] + w[ 5]*v[ 7] + w[ 6]*v[11] + w[ 7]*v[15];

	u[ 8] = w[ 8]*v[ 0] + w[ 9]*v[ 4] + w[10]*v[ 8] + w[11]*v[12];
	u[ 9] = w[ 8]*v[ 1] + w[ 9]*v[ 5] + w[10]*v[ 9] + w[11]*v[13];
	u[10] = w[ 8]*v[ 2] + w[ 9]*v[ 6] + w[10]*v[10] + w[11]*v[14];
	u[11] = w[ 8]*v[ 3] + w[ 9]*v[ 7] + w[10]*v[11] + w[11]*v[15];

	u[12] = w[12]*v[ 0] + w[13]*v[ 4] + w[14]*v[ 8] + w[15]*v[12];
	u[13] = w[12]*v[ 1] + w[13]*v[ 5] + w[14]*v[ 9] + w[15]*v[13];
	u[14] = w[12]*v[ 2] + w[13]*v[ 6] + w[14]*v[10] + w[15]*v[14];
	u[15] = w[12]*v[ 3] + w[13]*v[ 7] + w[14]*v[11] + w[15]*v[15];
}



void main()
{
	int x,y;
	float temp2[4];
	float matrix2[4] = {
		3.7, 0.8,
		-4.9, 2.5
	};
	float temp3[9];
	float matrix3[9] = {
		3.7, 0.8, 1.8,
		-4.9, 2.5, 0.1,
		3.3, 7.1, 5.5
	};

	matrixinverse_2(temp2, matrix2);
	for(y=0;y<2;y++)
	{
		for(x=0;x<2;x++)
		{
			printf("%f	", temp2[y*2+x]);
		}
		printf("\n");
	}
	printf("\n");

	matrixmultiply_2(temp2, matrix2);
	for(y=0;y<2;y++)
	{
		for(x=0;x<2;x++)
		{
			printf("%f	", temp2[y*2+x]);
		}
		printf("\n");
	}
	printf("\n");

	matrixinverse_3(temp3, matrix3);
	for(y=0;y<3;y++)
	{
		for(x=0;x<3;x++)
		{
			printf("%f	", temp3[y*3+x]);
		}
		printf("\n");
	}
	printf("\n");

	matrixmultiply_3(temp3, matrix3);
	for(y=0;y<3;y++)
	{
		for(x=0;x<3;x++)
		{
			printf("%f	", temp3[y*3+x]);
		}
		printf("\n");
	}
}