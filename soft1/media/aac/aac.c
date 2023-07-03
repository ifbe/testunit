#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;


#define BUFSZ 0x10000
static unsigned char buf[BUFSZ];


int aac_u(unsigned char* buf, int* pos, int cnt)
{
	int j,aa,bb;
	int ret = 0;
//printf("@h264_u:bitpos=%x,cnt=%x\n", *pos, cnt);
	for(j=0;j<cnt;j++){
		ret = ret<<1;
		aa = (*pos)/8;
		bb = (*pos)%8;
		if((buf[aa] & (0x80>>bb)) != 0)ret |= 1;
		*pos += 1;
	}
	return ret;
}
int parseaac(u8* buf)
{
	int pos = 0;
	printf("syncword=%x\n", aac_u(buf, &pos, 12) );
	printf("id=%x\n", aac_u(buf, &pos, 1) );
	printf("layer=%x\n", aac_u(buf, &pos, 2) );
	printf("proctection=%x\n", aac_u(buf, &pos, 1) );
	printf("profile=%x\n", aac_u(buf, &pos, 2) );
	printf("rate=%x\n", aac_u(buf, &pos, 4) );
	printf("private=%x\n", aac_u(buf, &pos, 1) );
	printf("channel=%x\n", aac_u(buf, &pos, 3) );
	printf("origincopy=%x\n", aac_u(buf, &pos, 1) );
	printf("home=%x\n", aac_u(buf, &pos, 1) );
	printf("copyright_bit=%x\n", aac_u(buf, &pos, 1) );
	printf("copyright_start=%x\n", aac_u(buf, &pos, 1) );
	int framesize = aac_u(buf, &pos, 13);
	printf("framesize=%x\n", framesize);
	printf("adts_buffer_fullness=%x\n", aac_u(buf, &pos, 11) );
	printf("num_of_data_block=%x\n", aac_u(buf, &pos, 2) );

	return framesize;
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./annexb /path/to/xxx.aac\n");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");
	if(0 == fp)goto error;

	int ret;
	int now = 0;
	int cnt = 0;
	while(1){
		printf("[%x,?)\n",now);
		ret = fseek(fp, now, SEEK_SET);
		//if(ret < 0)

		ret = fread(buf, 1, BUFSZ, fp);
		if(ret <= 0)break;

		ret = parseaac(buf);
		printf("[%x,%x)\n\n", now, now+ret);

		now += ret;
		cnt++;
		if(cnt > 10)break;
	}
good:
	printf("cnt=0x%x(%d)\n", cnt, cnt);
	fclose(fp);
	return 0;

error:
	return -1;
}
