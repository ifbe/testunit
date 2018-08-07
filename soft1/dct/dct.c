#include<math.h>
#include<stdio.h>
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230




void dct(float* src, float* dst, int N)
{
	int x,y;
	float temp;
	for(y=0;y<N;y++)
	{
		temp = 0.0;
		for(x=0;x<N;x++)
		{
			temp += src[x] * cos((x+0.5)*y*PI/N);
		}
		dst[y] = temp*sqrt(2.0/N);
	}
	dst[0] *= sqrt(1/2.0);
}
void idct(float* src, float* dst, int N)
{
	int x,y;
	float temp;
	float s0 = src[0]/sqrt(2.0);
	for(y=0;y<N;y++)
	{
		temp = s0;
		for(x=1;x<N;x++)
		{
			temp += src[x] * cos((y+0.5)*x*PI/N);
		}
		dst[y] = temp*sqrt(2.0/N);
	}
}




int main()
{
	int j;
	float src[64];
	float dst[64];
	for(j=0;j<64;j++)src[j] = (float)j;

	dct(src, dst, 64);
	for(j=0;j<64;j++)
	{
		//if(dst[j]<1.0 && dst[j] > -1.0)dst[j] = 0.0;
		printf("%f\n",dst[j]);
	}

	idct(dst, src, 64);
	for(j=0;j<64;j++)
	{
		printf("%f\n",src[j]);
	}
}
