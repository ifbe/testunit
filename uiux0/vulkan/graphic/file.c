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




int main()
{
	//init
	void* ins = vulkan_init(0, 0);
	if(0 == ins)return -1;

	//vulkan: device and swapchain
	void* dev = vulkan_device_create(0, 0);
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
