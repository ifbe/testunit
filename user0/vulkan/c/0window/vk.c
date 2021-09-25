#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
//0
static VkInstance instance;
static VkSurfaceKHR surface;
//
VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
VkDevice logicaldevice;




int vulkan_exit()
{
	vkDestroyInstance(instance, 0);
	return 0;
}
void* vulkan_init(int cnt, const char** ext)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, 0);

	VkLayerProperties availableLayers[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	printf("vkEnumerateInstanceLayerProperties:\n");

	int j;
	for(j=0;j<layerCount;j++){
		printf("%4d:%s\n", j, availableLayers[j].layerName);
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = cnt;
	createInfo.ppEnabledExtensionNames = ext;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = 0;
	if (vkCreateInstance(&createInfo, 0, &instance) != VK_SUCCESS) {
		printf("failed to create instance!");
	}	

	return instance;
}




int checkDeviceExtensionProperties(VkPhysicalDevice device) {
	uint32_t cnt;
	vkEnumerateDeviceExtensionProperties(device, 0, &cnt, 0);

	VkExtensionProperties ext[cnt];
	vkEnumerateDeviceExtensionProperties(device, 0, &cnt, ext);
	printf("vkEnumerateDeviceExtensionProperties:\n");

	int j;
	int ret = -1;
	for(j=0;j<cnt;j++) {
		printf("%4d:ver=%04d,str=%s\n", j, ext[j].specVersion, ext[j].extensionName);
		if(0 == strcmp(ext[j].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)){
			if(ret < 0)ret = j;
		}
	}
	printf("ret=%d\n",ret);
	return ret;
}
int checkPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, int* gg, int* pp){
	//printf("dev=%p\n",device);

	uint32_t cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, 0);

	VkQueueFamilyProperties fam[cnt];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, fam);
	printf("vkGetPhysicalDeviceQueueFamilyProperties:\n");

	int j;
	int graphicwhich = -1;
	int presentwhich = -1;
	VkBool32 graphicSupport = 0;
	VkBool32 presentSupport = 0;
	for(j=0;j<cnt;j++) {
		printf("%4d\n",j);
		if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			graphicSupport = 1;
			graphicwhich = j;
		}

		presentSupport = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &presentSupport);
		if(presentSupport){
			presentwhich = j;
		}

		if((graphicSupport > 0) && (presentSupport > 0)){
			if(gg)gg[0] = graphicwhich;
			if(pp)pp[0] = presentwhich;
			return 1;
		}
	}
	return 0;
}
int freephysicaldevice() {
	return 0;
}
int initphysicaldevice() {
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, 0);
	if(0 == count) {
		printf("error@vkEnumeratePhysicalDevices\n");
		return -1;
	}

	VkPhysicalDevice devs[count];
	vkEnumeratePhysicalDevices(instance, &count, devs);
	printf("vkEnumeratePhysicalDevices:\n");

	int j,phy=-1;
	int aa,bb;
	physicaldevice = VK_NULL_HANDLE;
	for(j=0;j<count;j++) {
		printf("physicaldevice %d{\n", j);
		aa = checkDeviceExtensionProperties(devs[j]);
		bb = checkPhysicalDeviceQueueFamilyProperties(devs[j], 0, 0);

		printf("aa=%d,bb=%d\n",aa,bb);
		if((aa >= 0)&&(bb > 0)){
			if(phy < 0)phy = j;
		}
		printf("}\n");
	}
	if(phy < 0){
		printf("no physicaldevice\n");
		return -2;
	}

	physicaldevice = devs[phy];
	printf("physicaldevice:id=%d,ptr=%p\n", phy, physicaldevice);
	return 0;
}




int vulkan_device_delete()
{
	freephysicaldevice();

	vkDestroySurfaceKHR(instance, surface, 0);

	return 0;
}
int vulkan_device_create(VkSurfaceKHR p)
{
	surface = p;

	initphysicaldevice();

	return 0;
}




void vulkan_myctx_create()
{
}
void vulkan_myctx_delete()
{
}




void drawframe()
{
}