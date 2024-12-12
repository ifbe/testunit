#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

void parseffc4(u8* buf, int off, int len)
{
	u8* p;
	u8* q;
	while(len > 16){
		printf("@%x\n", off);
		p = buf+off+1;
		q = p+16;
		printf("color=0x%x\n", buf[off]);
		printf("dict="
			"%x,%x,%x,%x,"
			"%x,%x,%x,%x,"
			"%x,%x,%x,%x,"
			"%x,%x,%x,%x\n",
			p[ 0],p[ 1],p[ 2],p[ 3],
			p[ 4],p[ 5],p[ 6],p[ 7],
			p[ 8],p[ 9],p[10],p[11],
			p[12],p[13],p[14],p[15]);
		printf("table=%x,%x,%x,%x...\n",q[0],q[1],q[2],q[3]);

		int j,k;
		int val = 0;
		int tmp = 0;
		for(j=0;j<16;j++){
			if(0 == p[j])continue;
			for(k=0;k<p[j];k++){
				printf("%2x: width=%x encoded=%x -> raw=%x\n", tmp, p[j], val+k, q[tmp]);
				tmp++;
			}
			val = (val+p[j])<<1;
		}

		printf("sz=0x%x\n", tmp);
		off += 1+16+tmp;
		len -= 1+16+tmp;
	}
}

struct app0{
	u32 magic;
	u8 b10;
	u8 version_major;
	u8 version_minor;
	u8 densitytype;
	u8 density_x;
	u8 density_y;
	u8 thumbnail_x;
	u8 thumbnail_y;
};
void parseffe0(struct app0* p){
	printf("magic=%.4s\n", (char*)&p->magic);
}

struct app1{
	u32 magic;
};
void parseffe1(struct app1* p){
	printf("magic=%.4s\n", (char*)&p->magic);
}

void parsedata(u8* tmp, int j){
	printf("%8x %02x%02x%02x%02x	data\n", j, tmp[j], tmp[j+1], tmp[j+2], tmp[j+3]);
}
unsigned char tmp[0x100000];
int main(int argc, char** argv){
	if(argc < 2){
		printf("./a.out xxx.jpg\n");
		return 0;
	}
	FILE* fp = fopen(argv[1], "r");
	int len = fread(tmp, 1, 0x100000, fp);

	int j = 0;
	int sz;
	int go = 1;
	while(go){
		if(j>=len){
			printf("j>=len\n");
			break;
		}

		printf("%8x %02x%02x____	", j, tmp[j], tmp[j+1]);
		switch(tmp[j+1]){
		case 0xc0:
			printf("sof0\n");
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
			break;
		case 0xc4:
			printf("dht\n");
			sz = (tmp[j+2]<<8)+tmp[j+3];
			parseffc4((void*)tmp, j+4, sz-2);
			j += 2+sz;
			break;
		case 0xd8:
			printf("soi\n");
			j += 2;
			break;
		case 0xd9:
			printf("eoi\n");
			j += 2;
			break;
		case 0xda:
			printf("sos\n");
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
			go = 0;
			break;
		case 0xdb:
			printf("dqt\n");
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
			break;
		case 0xe0:
			printf("app0\n");
			parseffe0((void*)tmp+j+4);
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
			break;
		case 0xe1:
			printf("app1\n");
			parseffe1((void*)tmp+j+4);
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
			break;
		default:
			printf("????\n");
			j += 2+(tmp[j+2]<<8)+tmp[j+3];
		}
	}

	if(0 == go){
		parsedata(tmp, j);
	}

	fclose(fp);
	return 0;
}
