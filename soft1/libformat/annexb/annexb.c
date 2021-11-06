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
int parsepacket(unsigned char* ptr, int len)
{
	if(0 != ptr[0])return -1;
	if(0 != ptr[1])return -2;
	if(2 <= ptr[2])return -3;

	else if(1 == ptr[2]){
		ptr += 3;
		len -= 3;
	}
	else if(0 == ptr[2]){
		if(1 != ptr[3])return -4;
		ptr +=4;
		len -=4;
	}

	printf("%x,%x,%x,%x\n", ptr[0],ptr[1],ptr[2],ptr[3]);

	int j;
	for(j=1;j<len-2;j++){
		if((0 == ptr[j])&&(0 == ptr[j+1])&&(3 == ptr[j+2])){
			printf("003@%x\n",j);
		}
	}

	return 0;
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

		printf("%x:[%x,%x]\n", cnt, now, now+ret);
		parsepacket(buf, ret);

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
