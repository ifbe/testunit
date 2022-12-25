#include <stdio.h>
#include <stdlib.h>
typedef unsigned int u32;
int mystrtox(char* p, u32* tmp)
{
	int j;
	u32 t = 0;
	for(j=0;j<8;j++){
		if(p[j] <= 0xa)break;
		if((p[j] >= '0')&&(p[j] <= '9')){
			t = (t<<4) + p[j] - '0';
			continue;
		}
		if((p[j] >= 'A')&&(p[j] <= 'F')){
			t = (t<<4) + p[j] + 10 - 'A';
			continue;
		}
		if((p[j] >= 'a')&&(p[j] <= 'f')){
			t = (t<<4) + p[j] + 10 - 'a';
			continue;
		}
		else return -1;
	}
	*tmp = t;
	return 0;
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out 33f20\n");
		return 0;
	}

	u32 x;
	int ret = mystrtox(argv[1], &x);	//0x46d0;
	if(ret < 0){
		printf("wrong hex\n");
		return 0;
	}
	printf("x=%x\n", x);

	printf("%x\n", (~(x-1)) & x);
	return 0;
}
