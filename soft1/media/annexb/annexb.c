#include <stdio.h>
#include <stdlib.h>
int parseh264(unsigned char* buf, int len);
int parseh265(unsigned char* buf, int len);
int parseh266(unsigned char* buf, int len);


#define BUFSZ 0x200000
static unsigned char buf[BUFSZ];


int find0001(unsigned char* ptr, int len)
{
	int j;
	for(j=4;j<len;j++){
		if( (0 == buf[j+0])&&(0 == buf[j+1]) )
		{
			if(1 == buf[j+2])return j;
			if((0 == buf[j+2])&&(1 == buf[j+3]))return j;
		}
	}
	return -1;
}
int ebsp2rbsp(unsigned char* ptr, int len, unsigned char* dst, int max)
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


	int j,k=0;
	for(j=0;j<len-2;j++){
		if((0 == ptr[j])&&(0 == ptr[j+1])&&(3 == ptr[j+2])){
			//printf("003@%x\n",j);

			dst[k+0] = 0;
			dst[k+1] = 0;
			k += 2;
			j += 2;
			continue;
		}
		dst[k] = ptr[j];
		k++;
	}
	dst[k] = ptr[j];
	dst[k+1] = ptr[j+1];
	k += 2;

	return k;
}




int main(int argc, char** argv)
{
	if(argc < 3){
		printf("./annexb h265 /path/to/xxx.h264\n");
		return 0;
	}

	int codec = argv[1][0];

	FILE* fp = fopen(argv[2], "rb");
	if(0 == fp)goto error;

	int ret,tmp;
	int now = 0;
	int cnt = 0;
	while(1){
		ret = fseek(fp, now, SEEK_SET);
		//if(ret < 0)

		ret = fread(buf, 1, BUFSZ, fp);
		if(ret <= 0)break;

		ret = find0001(buf, ret);
		if(ret <= 0)break;
		printf("%x@[%x,%x]:%x,%x,%x,%x,%x,%x,%x,%x\n", cnt, now, now+ret,
			buf[0],buf[1],buf[2],buf[3],
			buf[4],buf[5],buf[6],buf[7]);

		tmp = ebsp2rbsp(buf, ret, buf, ret);
		//printf("ebsp2rbsp:%x->%x\n", ret, tmp);
		printf("%x,%x,%x,%x,%x,%x,%x,%x\n",
			buf[0],buf[1],buf[2],buf[3],
			buf[4],buf[5],buf[6],buf[7]);

		switch(codec){
		case '4':
			parseh264(buf, tmp);
			break;
		case '5':
			parseh265(buf, tmp);
			break;
		case '6':
			parseh266(buf, tmp);
			break;
		}

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
