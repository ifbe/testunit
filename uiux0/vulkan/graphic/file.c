#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void* vulkan_init(int cnt, const char** ext);
void vulkan_exit();
void* vulkan_device_create(int, void*);
void vulkan_device_delete();
void vulkan_myctx_create();
void vulkan_myctx_delete();
void drawframe();




static int mycb(unsigned char* buf, int pitch, int w, int h)
{
	printf("buf=%p,pitch=0x%x,w=%d,h=%d\n", buf, pitch, w, h);
	FILE* fp = fopen("out.ppm", "wb");

	char tmp[0x100];
	int ret = snprintf(tmp, 0x100, "P6\n%d\n%d\n255\n", w, h);
	fwrite(tmp, 1, ret, fp);

	int x,y;
	unsigned char* row;
	for(y = 0; y < h; y++) {
		row = buf + pitch*y;
		for(x = 0; x < w; x++) {
			fwrite(row+2, 1, 1, fp);
			fwrite(row+1, 1, 1, fp);
			fwrite(row+0, 1, 1, fp);
			row += 4;
		}
	}

	fclose(fp);
	return 0;
}
int main()
{
	//init
	void* ins = vulkan_init(0, 0);
	if(0 == ins)return -1;

	//vulkan: device and swapchain
	void* dev = vulkan_device_create(0, mycb);
	if(0 == dev)return -2;

	//vulkan: things
	vulkan_myctx_create();

	//once
	drawframe();

	//vulkan
	vulkan_myctx_delete();

	//
	vulkan_device_delete();

	//exit
	vulkan_exit();
	return 0;
}
