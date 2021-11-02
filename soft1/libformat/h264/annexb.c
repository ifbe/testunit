#include <stdio.h>
#include <stdlib.h>


static unsigned char buf[0x100000];


int find0001(unsigned char* ptr, int len)
{
	int j;
	for(j=4;j<len;j++){
		if(	(0 == buf[j+0])&&
			(0 == buf[j+1])&&
			(0 == buf[j+2])&&
			(1 == buf[j+3]) )
		{
			return j;
		}
	}
	return -1;
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out /path/to/xxx.h264\n");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");
	if(0 == fp)goto error;

	int ret;
	int now = 0;
	int cnt = 0;
	while(1){
		ret = fseek(fp, now, SEEK_SET);
		//if(ret < 0)

		ret = fread(buf, 1, 0x100000, fp);
		if(ret <= 0)break;

		ret = find0001(buf, ret);
		if(ret <= 0)break;

		printf("[%x,%x]\n", now, now+ret);
		now += ret;
		cnt++;
	}
good:
	printf("cnt=0x%x(%d)\n", cnt, cnt);
	fclose(fp);
	return 0;

error:
	return -1;
}
