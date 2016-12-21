#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230




static unsigned int buf0[1024];
static float buf1[1024];	//real
static float buf2[1024];	//imag




void fft(float real[], float imag[], int k)
{
	float tempr, tempi, facr, faci;
	int i, j, k1, k2, m, step, factor_step;
	int N = 1<<k;

	for(j=1;j<N-1;j++)
	{
		//reverse bit
		i=0;
		k1 = j;
		for(m=0;m<k;m++)
		{
			i <<= 1;
			i += (k1&1);
			k1 >>= 1;
		}

		//only small -> big
		if(j>=i)continue;

		//swap data
		tempr = real[j];
		real[j] = real[i];
		real[i] = tempr;

		tempi = imag[j];
		imag[j] = imag[i];
		imag[i] = tempi;
	}

	//butterfly computation
	for( i = 0; i < k; i++)
	{
		step = 1 << (i+1);
		factor_step = N >> (i+1);

		facr = 1.0;
		faci = 0.0;
		for(j = 0; j < step/2; j++)
		{
			for(k1 = j; k1 < N; k1 += step)
			{
				k2 = k1 + step/2;
				tempr = real[k1] + real[k2]*facr - imag[k2]*faci;
				tempi = imag[k1] + real[k2]*faci + imag[k2]*facr;
				real[k2] = real[k1] - (real[k2]*facr - imag[k2]*faci);
				imag[k2] = imag[k1] - (real[k2]*faci + imag[k2]*facr);
				real[k1] = tempr;
				imag[k1] = tempi;
			}
			facr = cos(-2*PI*(j+1)*factor_step/N);
			faci = sin(-2*PI*(j+1)*factor_step/N);
		}
	}
}

void ifft(float real[], float imag[], int k)
{
	int i;
	int N = 1<<k;
	for(i=0;i<N;i++)
	{
		imag[i] = -imag[i];
	}
	fft(real, imag, k);
	for(i=0;i<N;i++)
	{
		real[i] = real[i]/N;
		imag[i] = -imag[i]/N;
	}
}

int main()
{
	int j;
	FILE* fp = fopen("test.pcm","r");
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fclose(fp);

	for(j = 0; j < 1024; j++)
	{
		buf1[j] = (float)(buf0[j]&0xffff)/65536.0;
		printf("%f	%f\n", buf1[j], buf2[j]);
	}
	printf("\n");

	//
	fft(buf1, buf2, 10);
	for(j = 0; j < 1024; j++)
	{
		//printf("%f	%f\n", buf1[j], buf2[j]);
		printf("%f\n", 20*log10(buf1[j]*buf1[j] + buf2[j]*buf2[j]) );
	}
	printf("\n");

	//
	ifft(buf1, buf2, 10);
	for(j = 0; j < 1024; j++)
	{
		printf("%f	%f\n", buf1[j], buf2[j]);
	}

	return 0;
}
