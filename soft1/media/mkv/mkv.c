#include "stdio.h"
#include "stdlib.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define hex32(a,b,c,d) (a|(b<<8)|(c<<16)|(d<<24))
char* tab = "								";




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


int mkv_vint(u8* buf, u64* val)
{
	u8 t = buf[0];
	int cnt = 0;
	for(cnt=0;cnt<8;cnt++){
		if(t&0x80)break;
		t <<= 1;
	}

	int j;
	*val = (t&0x7f) >> cnt;
	for(j=0;j<cnt;j++){
		*val <<= 8;
		*val |= buf[j+1];
	}

	return cnt+1;
}
int mkv_ebml(FILE* fp, u64 j, u64 k, u8* buf, int depth)
{
	int ret, offs;
	u64 id, sz;
	u64 tmp;
	while(1){
		//printf("j=%llx,k=%llx\n",j,k);
		if(j >= k)break;

		ret = fseek(fp, j, SEEK_SET);
		//if(ret <= 0)break;

		ret = fread(buf, 1, 0x10000, fp);
		if(ret <= 0)break;

		offs = mkv_vint(buf, &id);
		offs += mkv_vint(buf+offs, &sz);
		tmp = j+offs+sz;
		printf("%.*s[%llx,%llx)id=%llx,sz=%llx\n", depth, tab, j, tmp, id, sz);

		switch(id){
		case 0x8538067:	//Segment
		case 0xf43b675:	//Cluster
			mkv_ebml(fp, j+offs, j+offs+sz, buf, depth+1);
			break;
		case 0x282:	//DocType		//'matroska'
			printf("%.*sDocType='%.*s'\n", depth, tab, (int)sz, buf+offs);
			break;
		case 0x285:	//DocTypeReadVersion
			printf("%.*sDocTypeReadVersion=%d\n", depth, tab, buf[offs]);
			break;
		case 0x286:	//EBMLVersion
			printf("%.*sEBMLVersion=%d\n", depth, tab, buf[offs]);
			break;
		case 0x287:	//DocTypeVersion
			printf("%.*sDocTypeVersion=%d\n", depth, tab, buf[offs]);
			break;
		case 0x2f2:	//EBMLMaxIDLength
			printf("%.*sEBMLMaxIDLength=%d\n", depth, tab, buf[offs]);
			break;
		case 0x2f3:	//EBMLMaxSizeLength
			printf("%.*sEBMLMaxSizeLength=%d\n", depth, tab, buf[offs]);
			break;
		case 0x2f7:	//EBMLReadVersion
			printf("%.*sEBMLReadVersion=%d\n", depth, tab, buf[offs]);
			break;
		default:
			printf("%.*s", depth, tab);
			print8(buf+offs, 8);
			break;
		}

		j = tmp;
	}
	return 0;
}
int main(int argc, char** argv)
{
	unsigned char tmp[0x10000];
	if(argc < 2){
		printf("./a.out /path/test.mkv\n");
		return 0;
	}

	FILE* fp = fopen(argv[1],"rb");
	if(!fp)return 0;

	int ret = fread(tmp, 1, 0x200, fp);
	if(ret < 16)return 0;

	print8(tmp, 8);

	mkv_ebml(fp, 5, 0xffffffff, tmp, 0);

	fclose(fp);
	return 0;
}
