#include <stdio.h>
#include <math.h>
#define PI 3.1415926
void fft(double real[], double imag[], int N, int k, int inv)
{
	int i, j, k1, k2, m, step, factor_step;
	double temp_real, temp_imag, factor_real, factor_imag;

	if(inv != 1 && inv != -1)
	{
		fprintf(stderr, "FFT transformation require: inv = 1\n");
		fprintf(stderr, "FFT inverse transformation require: inv = -1\n");
		return;
	}

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
			factor_real = inv*cos(-2*PI*(j+1)*factor_step/N);
			factor_imag = inv*sin(-2*PI*(j+1)*factor_step/N);
		}
	}
	if(inv == -1)
	{
		for(i = 0; i <= N-1; i++)
		{
			real[i] = real[i]/N;
			imag[i] = imag[i]/N;
		}
	}
	return;
}

int main()
{
int i = 0;
	double real[32] = {
		39.000000,39.000000,52.000000,25.000000,
		30.000000,28.000000,31.000000,35.000000,
		36.000000,48.000000,69.000000,37.000000,
		61.000000,101.000000,79.000000,67.000000,
		81.000000,81.000000,111.000000,109.000000,
		132.000000,102.000000,88.000000,92.000000,
		53.000000,45.000000,46.000000,41.000000,
		46.000000,37.000000,45.000000,34.000000
	};
	double imag[32] = {
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0
	};

	fft(real, imag, 32, 5, 1);
	for(i = 0; i < 32; i++)
	{
		printf("%f + %f * i\n", real[i], imag[i]);
	}
	return 1;
}
