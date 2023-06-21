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


void parsedeccfg(u8* p, int l, int foff, int flen)
{
	printf("		[%x,%x)deccfg: %x,%x,%x,%x,%x,%x,%x,%x\n", foff, foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}
void parsenalu(u8* p, int l, int foff, int flen)
{
	int cnt = 0;
	int curr = 0, size = 0;
	do{
		size = (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | (p[3]);
		printf("		[%x,%x)nalu %d: %x,%x,%x,%x,%x,%x,%x,%x\n",
			foff+curr, foff+curr+size+4, cnt,
			p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]
		);
		p += size+4;
		curr += size+4;
		cnt++;
	}while(curr < l);
}


struct flvtag_video{
	u8 frametype:4;
	u8 codecid:4;
}__attribute__((packed));
struct flvtag_video_h264{
	u8 codecid:4;	//lo4
	u8 frametype:4;	//hi4
	u8 avc_type;
	u8 avc_cts[3];
	u8 avc_data[];
}__attribute__((packed));
void parsevideo(u8* p, int l, int foff, int flen)
{
	//printf("%x,%x,%x,%x,%x,%x,%x,%x\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

	struct flvtag_video_h264* tagvideo = (void*)p;
	u8 frametype = tagvideo->frametype;
	u8 codecid = tagvideo->codecid;

	u8 avc_type;
	int avc_dt;
	struct flvtag_video_h264* tagh264 = (void*)p;
	switch(codecid){
	case 7:
		avc_type = tagh264->avc_type;
		avc_dt = ((int)tagh264->avc_cts[0]<<16) | (tagh264->avc_cts[1]<<8) | (tagh264->avc_cts[2]);
		printf("	[%x,%x)h264: frametype=%x,codecid=%x,avc_type=%x,avc_dt=%d: ",
			foff, foff+flen,
			frametype, codecid,
			avc_type, avc_dt
		);
		print8(p, 8);
		switch(avc_type){
		case 0:
			parsedeccfg(tagh264->avc_data, flen-5, foff+5, flen-5);
			break;
		case 1:
			parsenalu(tagh264->avc_data, flen-5, foff+5, flen-5);
			break;
		case 2:
			printf("		end of avc: ");
			print8(tagh264->avc_data, flen-5);
			break;
		default:
			print8(tagh264->avc_data, flen-5);
			break;
		}
		break;
	case 12:
		printf("	[%x,%x)h265: frametype=%x,codecid=%x: ",foff,foff+flen,frametype, codecid);
		print8(p, 8);
	default:
		printf("	[%x,%x)video: frametype=%x,codecid=%x: ",foff,foff+flen,frametype, codecid);
		print8(p, 8);
		break;
	}
}


void parseaudio(u8* p, int l, int foff, int flen)
{
	printf("	[%x,%x)audio: %x,%x,%x,%x,%x,%x,%x,%x\n",
		foff,foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}


void parsescript(u8* p, int l, int foff, int flen)
{
	printf("	[%x,%x)script: %x,%x,%x,%x,%x,%x,%x,%x\n",
		foff,foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}


struct flvhead{
	u8 flv[3];
	u8 ver;
	u8 attr;
	u32 size;	//0,0,0,9
	//u8 prevsize[4];
}__attribute__((packed));
struct flvtag{
	u8 type;
	u8 thissize[3];
	u8 time[3];
	u8 time_ext;
	u8 stream[3];
	u8 data[0];
	//u8 prevsize[4];
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

	u32 type,thissize,time,stream;
	struct flvtag* tag = (void*)tmp;

	int prevsize;
	u8* ptr;
	while(1){
		ret = fseek(fp, j, SEEK_SET);
		ret = fread(tmp, 1, 0x10000, fp);
		if(ret < 11)break;

		type = tag->type;
		thissize = (tag->thissize[0]<<16) | (tag->thissize[1]<<8) | tag->thissize[2];
		time = (tag->time[0]<<16) | (tag->time[1]<<8) | tag->time[2];
		stream = (tag->stream[0]<<16) | (tag->stream[1]<<8) | tag->stream[2];

		k = j+11+thissize;
		printf("[%x,%x): type=%x,thissize=%x,time=%.3f,stream=%x\n", j,k, type,thissize,(float)time*0.001,stream);
		switch(type){
		case 9:
			parsevideo(tag->data, thissize, j+11, k-j-11);
			break;
		case 8:
			parseaudio(tag->data, thissize, j+11, k-j-11);
			break;
		case 18:
			parsescript(tag->data, thissize, j+11, k-j-11);
			break;
		}

		ptr = tmp+11+thissize;
		prevsize = (ptr[0]<<24) | (ptr[1]<<16) | (ptr[2]<<8) | ptr[3];
		printf("[%x,%x): prevsize=%x(b+%x)\n\n",k,k+4,prevsize,prevsize-11);

		j = k+4;	//prevsize at the end
	}

	fclose(fp);
	return 0;
}
