#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void* vulkan_init(int cnt, const char** ext);
void vulkan_exit();
void vulkan_device_create(void*);
void vulkan_device_delete();
void vulkan_myctx_create();
void vulkan_myctx_delete();
void drawframe();




int main()
{
	//init
	void* instance = vulkan_init(0, 0);

	//vulkan: device and swapchain
	vulkan_device_create(0);

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
