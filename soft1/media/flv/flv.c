#include "stdio.h"
#include "stdlib.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define hex32(a,b,c,d) (a|(b<<8)|(c<<16)|(d<<24))




u16 swap16(u16 in)
{
	return ((in<<8)&0xff00) | ((in>>8)&0xff);
}
u32 swap32(u32 in)
{
	return (in>>24) | ((in>>8)&0xff00) | ((in<<8)&0xff0000) | (in<<24);
}
u64 swap64(u64 in)
{
	u64 lo = in&0xffffffff;
	u64 hi = (in>>32)&0xffffffff;
	lo = swap32(lo);
	hi = swap32(hi);
	return (lo<<32) | hi;
}
void print8(void* buf, int len)
{
	u8* p = buf;
	printf("%x,%x,%x,%x,%x,%x,%x,%x\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}




struct flvhead{
	u8 flv[3];
	u8 ver;
	u8 attr;
	u32 size;	//0,0,0,9
}__attribute__((packed));
struct flvtag{
	u8 type;
	u8 size[3];
	u8 time[3];
	u8 time_ext;
	u8 stream[3];
	u8 data[0];
}__attribute__((packed));
int main(int argc, char** argv)
{
	unsigned char tmp[0x10000];
	if(argc < 2){
		printf("./a.out /path/flv.mp4 3.14\n");
		return 0;
	}

	float pts = 0.0;
	if(3 == argc){
		pts = atof(argv[2]);
		printf("pts=%f\n", pts);
	}

	FILE* fp = fopen(argv[1],"rb");
	if(!fp)return 0;

	int ret = fread(tmp, 1, 0x100, fp);
	if(ret < 9)return 0;

	struct flvhead* head = (void*)tmp;
	int hlen = swap32(head->size);
	printf("ver=%x,attr=%x,size=%x\n", head->ver, head->attr, hlen);

	int j = hlen+4, k = 0;
	u32 type,size,time,stream;
	struct flvtag* tag = (void*)tmp;
	while(1){
		ret = fseek(fp, j, SEEK_SET);
		ret = fread(tmp, 1, 0x1000, fp);
		if(ret < 11)break;

		type = tag->type;
		size = (tag->size[0]<<16) | (tag->size[1]<<8) | tag->size[2];
		time = (tag->time[0]<<16) | (tag->time[1]<<8) | tag->time[2];
		stream = (tag->stream[0]<<16) | (tag->stream[1]<<8) | tag->stream[2];

		k = j+11+size+4;
		printf("[%x,%x]: type=%x,size=%x,time=%x,stream=%x\n", j,k, type,size,time,stream);

		j = k;
	}

	fclose(fp);
	return 0;
}
