#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned short u16;
void* vulkan_init(int cnt, const char** ext);
void vulkan_exit();
void vulkan_myctx_create(void* surface, void* callback);
void vulkan_myctx_delete();
void drawframe();




char* table = ".'`^\",:;Il!i><~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
int ascii4y(int y)
{
	return table[((y&0xff)*0x44)>>8];
}
void rgbx_to_ascii(
	u8* srcbuf, int srclen, int srcw, int srch)
{
	int x,y,dx,dy;
	int avgy,avgu,avgv;
	int avgr,avgg,avgb;
	for(y=0;y<32;y++){
		for(x=0;x<64;x++){
			if(x*16+8 > srcw)continue;
			if(y*32+16 > srch)continue;
			avgr = 0;
			avgg = 0;
			avgb = 0;
			for(dy=0;dy<32;dy++){
				for(dx=0;dx<16;dx++){
					avgr += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+0];
					avgg += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+1];
					avgb += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+2];
				}
			}
			avgr = avgr >> 7;
			avgg = avgg >> 7;
			avgb = avgb >> 7;
			avgy = ( 77*avgr +150*avgg + 29*avgb)>>8;
			printf("%c",ascii4y(avgy));
		}
		printf("\n");
	}
}
static int mycb(void* buf, int pitch, int w, int h)
{
	printf("buf=%p,pitch=0x%x,w=%d,h=%d\n", buf, pitch, w, h);
	rgbx_to_ascii(buf, pitch, w, h);
	return 0;
}
int main()
{
	//init
	void* ins = vulkan_init(0, 0);
	if(0 == ins)return -1;

	//vulkan: things
	vulkan_myctx_create(0, mycb);

	//once
	drawframe();

	//vulkan
	vulkan_myctx_delete();

	//exit
	vulkan_exit();
	return 0;
}
