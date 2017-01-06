#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230
#define tau PI*2




static unsigned int buf0[1024];
static double buf1[1024];	//real
static double buf2[1024];	//imag




//W(kn, N) = cos(kn*2PI/N) - jsin(kn*2PI/N)
void fft(double real[], double imag[], int k)
{
	double re1, im1, facr, faci;
	int i, j, m, n, step, factor;
	int len = 1<<k;

	for(j=1;j<len-1;j++)
	{
		//reverse bit
		i=0;
		n = j;
		for(m=0;m<k;m++)
		{
			i <<= 1;
			i += (n&1);
			n >>= 1;
		}

		//only small -> big
		if(j>=i)continue;
//printf("%x,%x\n",j,i);
		//swap data
		re1 = real[j];
		real[j] = real[i];
		real[i] = re1;

		im1 = imag[j];
		imag[j] = imag[i];
		imag[i] = im1;
	}

	//总共k级
	for(i = 0; i < k; i++)
	{
		step = 1 << i;
		factor = len >> (i+1);
		facr = 1.0;
		faci = 0.0;

		//每组第1个，第2个，第3个(重用factor)
		for(j = 0; j < step; j++)
		{
			//第0次512组，第1次256组，第2次128组
			for(m = j; m < len; m += step*2)
			{
				n = m + step;
				re1 = real[n]*facr - imag[n]*faci;
				im1 = real[n]*faci + imag[n]*facr;

				real[n] = real[m] - re1;
				imag[n] = imag[m] - im1; 
				real[m] += re1;
				imag[m] += im1;
			}

			//算下一次的（第一次不乘）
			facr = cos(2*PI*(j+1)*factor/len);
			faci = -sin(2*PI*(j+1)*factor/len);
		}
	}

}

void ifft(double real[], double imag[], int k)
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
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fread(buf0, 4, 1024, fp);
	fclose(fp);

	for(j = 0; j < 1024; j++)
	{
		//buf1[j] = sin((double)j*tau/8);
		buf1[j] = (double)(buf0[j]&0xffff)/65536.0;
		buf2[j] = 0.0;
		printf("%f	%f\n", buf1[j], buf2[j]);
		//printf("%f\n", buf1[j]);
	}
	printf("\n");

	//
	fft(buf1, buf2, 10);
	for(j = 0; j < 1024; j++)
	{
		//printf("%f\n", buf1[j]);
		printf("%f\n", 10*log10(buf1[j]*buf1[j] + buf2[j]*buf2[j]) );
	}
	printf("\n");

	//
	ifft(buf1, buf2, 10);
	for(j = 0; j < 1024; j++)
	{
		printf("%f	%f\n", buf1[j], buf2[j]);
		//printf("%f\n", buf1[j]);
	}

	return 0;
}
