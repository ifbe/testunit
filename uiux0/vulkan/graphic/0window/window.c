#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
void* vulkan_device_create(int, void*);
void vulkan_device_delete(void*);




void* device = 0;
int vulkan_myctx_create(void* surface, void* cb)
{
	//vulkan: device and swapchain
	device = vulkan_device_create(1, surface);
	if(0 == device)return -4;

	return 0;
}
int vulkan_myctx_delete()
{
	//vulkan device
	vulkan_device_delete(device);
	return 0;
}




void drawframe()
{
}
