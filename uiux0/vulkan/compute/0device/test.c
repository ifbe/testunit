#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
void* vulkan_device_create(int, void*);
void vulkan_device_delete(void*);
void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev);




VkPhysicalDevice physicaldevice = 0;
VkDevice logicaldevice = 0;
//
VkQueue computeQueue;
VkCommandPool computePool;




void vulkan_myctx_create()
{
        physicaldevice = vulkan_device_create(2, 0);
        if(0 == physicaldevice)return;

	vulkan_physicaldevice_logicdevice(&physicaldevice, &logicaldevice);
}
void vulkan_myctx_delete()
{
	vulkan_device_delete(physicaldevice);
}




void drawframe()
{
}
