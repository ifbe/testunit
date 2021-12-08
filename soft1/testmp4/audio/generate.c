#include <stdio.h>
#include <math.h>
#define PI 3.141592653
#define KI 1.0594630943592953


short pcm[3*44100];	//3s


int main(int argc, char** argv)
{
	int j,k;
	int freq = 440;
	for(j=0;j<44100*3;j++)pcm[j] = 0;

	for(k=1;k<29;k++){
		for(j=0;j<1000;j++){
			pcm[k*4410-500+j] = 32767*sin(j*PI/1000)*sin(j*2*PI*freq/44100);
		}
		freq *= KI;
	}

	FILE* fp = fopen("audio.pcm","wb");
	fwrite(pcm, 1, 2*44100*3, fp);
	fclose(fp);
	return 0;
}
