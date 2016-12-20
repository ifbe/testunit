#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230




static float buf0[1024];	//sample
static float buf1[1024];	//real
static float buf2[1024];	//imag




void fft(float real[], float imag[], float sample[], int k)
{
	float temp_real, temp_imag, factor_real, factor_imag;
	int i, j, k1, k2, m, step, factor_step;
	int N = 1<<k;

	//invert order
	j = 0;
	for ( i = 0; i < N; i ++)
	{
		if(j > i)
		{
			temp_real = real[j];
			real[j] = real[i];
			real[i] = temp_real;

			temp_imag = imag[j];
			imag[j] = imag[i];
			imag[i] = temp_imag;
		}

		//?
		m = N/2;
		while(j >= m && m != 0)
		{
			j-=m;
			m>>=1;
		}
		j += m;
	}

	//butterfly computation
	for( i = 0; i < k; i++)
	{
		step = 1 << (i+1);
		factor_step = N >> (i+1);

		factor_real = 1.0;
		factor_imag = 0.0;
		for(j = 0; j < step/2; j++)
		{
			for(k1 = j; k1 < N; k1 += step)
			{
				k2 = k1 + step/2;
				temp_real = real[k1] + real[k2]*factor_real - imag[k2]*factor_imag;
				temp_imag = imag[k1] + real[k2]*factor_imag + imag[k2]*factor_real;
				real[k2] = real[k1] - (real[k2]*factor_real - imag[k2]*factor_imag);
				imag[k2] = imag[k1] - (real[k2]*factor_imag + imag[k2]*factor_real);
				real[k1] = temp_real;
				imag[k1] = temp_imag;
			}
			factor_real = cos(-2*PI*(j+1)*factor_step/N);
			factor_imag = sin(-2*PI*(j+1)*factor_step/N);
		}
	}
}

void ifft(float real[], float imag[], float sample[], int k)
{
	int i;
	int N = 1<<k;
	for(i=0;i<N;i++)
	{
		imag[i] = -imag[i];
	}
	fft(real, imag, sample, k);
	for(i=0;i<N;i++)
	{
		real[i] = real[i]/N;
		imag[i] = -imag[i]/N;
	}
}

int main()
{
	int j;
	for(j = 0; j < 1024; j++)
	{
		buf1[j] = (float)(rand()%32768)/32768.0;
		printf("%f	%f\n", buf1[j], buf2[j]);
	}
	printf("\n");

	//
	fft(buf1, buf2, buf0, 10);
	for(j = 0; j < 1024; j++)
	{
		printf("%f	%f\n", buf1[j], buf2[j]);
	}
	printf("\n");

	//
	ifft(buf1, buf2, buf0, 10);
	for(j = 0; j < 1024; j++)
	{
		printf("%f	%f\n", buf1[j], buf2[j]);
	}

	return 0;
}
