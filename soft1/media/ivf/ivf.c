#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


struct ivfhdr{
	u32 DKIF;
	u16 ver;
	u16 len;
	u32 fourcc;
	u16 width;
	u16 height;
	u32 framerate;
	u32 timescale;
	u32 framecount;
	u32 unused;
}__attribute__((packed));
int ivfparser_header(FILE* fp, int off, struct ivfhdr* pkt, int len)
{
	int ret = fseek(fp, off, SEEK_SET);
	//if(ret < 0)

	ret = fread(pkt, 1, 0x100, fp);
	if(ret <= 0)return 0;

	printf("sig=%.4s\n", (char*)&pkt->DKIF);
	printf("ver=%x\n", pkt->ver);
	printf("len=%x\n", pkt->len);
	printf("fource=%.4s\n", (char*)&pkt->fourcc);
	printf("width=%x(%d)\n", pkt->width, pkt->width);
	printf("height=%x(%d)\n",pkt->height,pkt->height);
	printf("fps=%d=%x/%x\n", pkt->framerate/pkt->timescale, pkt->framerate, pkt->timescale);
	printf("count=%x\n",pkt->framecount);
	return pkt->len;
}




struct ivfpkt{
	u32 len;
	u64 time;
	u8 ptr[];
}__attribute__((packed));
int ivfparser_packet(FILE* fp, int off, struct ivfpkt* pkt, int len)
{
	int ret = fseek(fp, off, SEEK_SET);
	//if(ret < 0)

	ret = fread(pkt, 1, 0x20, fp);
	if(ret <= 0)return 0;

	printf("[%x,%x)size=%x\n", off+12, off+12+pkt->len, pkt->len);
	printf("time=%llx\n", pkt->time);
	printf("data=%x,%x,%x,%x,%x,%x,%x,%x\n",
		pkt->ptr[0], pkt->ptr[1], pkt->ptr[2], pkt->ptr[3],
		pkt->ptr[4], pkt->ptr[5], pkt->ptr[6], pkt->ptr[7]);
	return 12+pkt->len;
}




int main(int argc, char** argv)
{
	u8 buf[0x1000];
	if(argc < 2){
		printf("./a.out /path/to/xxx.ivf\n");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");
	if(0 == fp)goto error;

	int ret;
	int cnt = 0;
	int off = ivfparser_header(fp, 0, (void*)buf, 0);
	while(1){
		ret = ivfparser_packet(fp, off, (void*)buf, 0);
		if(ret <= 0)break;

		off += ret;
		cnt++;
	}
good:
	printf("cnt=0x%x(%d)\n", cnt, cnt);
	fclose(fp);
	return 0;

error:
	return -1;
}
