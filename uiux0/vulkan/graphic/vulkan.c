#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};




//instance
static VkInstance instance;
//device
static VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
static VkDevice logicaldevice;
//compute
static int computeindex;
static VkQueue computeQueue;
static VkCommandPool computePool;
//graphic
static int graphicindex;
static VkQueue graphicQueue;
static VkCommandPool graphicPool;
//present
static int presentindex;
static VkQueue presentQueue;
//surface,swapchain
static VkSurfaceKHR surface = 0;
static VkSwapchainKHR swapChain = 0;
static VkExtent2D widthheight;
static uint32_t imagecount;
//attachment
struct attachment{
	VkFormat format;
	VkDeviceMemory memory;
	VkImage image;
	VkImageView view;
};
static struct attachment attachcolor[8];




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




int checkDeviceProperties(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(device, &prop);

	int good = -1;
	printf("vkGetPhysicalDeviceProperties:\n");
	printf("	apiver=%x\n", prop.apiVersion);
	printf("	drvver=%x\n", prop.driverVersion);
	printf("	vendor=%x\n", prop.vendorID);
	printf("	device=%x\n", prop.deviceID);
	printf("	name=%s\n", prop.deviceName);
	printf("	type=");
	switch(prop.deviceType){
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		printf("other\n");
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		printf("igpu\n");
		good = 1;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		printf("dgpu\n");
		good = 1;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		printf("vgpu\n");
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		printf("cpu\n");
		break;
	default:
		printf("unknown\n");
		break;
	}

	printf("\n");
	return good;
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
		printf("%d:	ver=%04d,str=%s\n", j, ext[j].specVersion, ext[j].extensionName);
		if(0 == strcmp(ext[j].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)){
			if(ret < 0)ret = j;
		}
	}
	printf("=>VK_KHR_swapchain@%d\n\n",ret);
	return ret;
}
//#define VK_QUEUE_GRAPHICS_BIT         0x00000001
//#define VK_QUEUE_COMPUTE_BIT          0x00000002
//#define VK_QUEUE_TRANSFER_BIT         0x00000004
#define VK_QUEUE_SPARSE_BINDING_BIT   0x00000008
#define VK_QUEUE_PROTECTED_BIT        0x00000010
#define VK_QUEUE_VIDEO_DECODE_BIT_KHR 0x00000020
#define VK_QUEUE_VIDEO_ENCODE_BIT_KHR 0x00000040
int checkPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, int* gg, int* pp){
	//printf("dev=%p\n",device);

	uint32_t cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, 0);

	VkQueueFamilyProperties fam[cnt];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, fam);

	int j;
	int bestpresent = -1;
	int firsttransfer = -1;
	int firstcompute = -1;
	int firstgraphic = -1;
	int firstpresent = -1;
	VkBool32 support = 0;
	printf("vkGetPhysicalDeviceQueueFamilyProperties:\n");
	for(j=0;j<cnt;j++) {
		printf("%d:	%d=", j, fam[j].queueFlags);
		if(fam[j].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR){	//64
			printf("encode,");
		}
		if(fam[j].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR){	//32
			printf("decode,");
		}
		if(fam[j].queueFlags & VK_QUEUE_PROTECTED_BIT){	//16
			printf("protected,");
		}
		if(fam[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT){	//8
			printf("sprase,");
		}
		if(fam[j].queueFlags & VK_QUEUE_TRANSFER_BIT){	//4
			printf("transfer,");
			if(firsttransfer < 0)firsttransfer = j;
		}
		if(fam[j].queueFlags & VK_QUEUE_COMPUTE_BIT){	//2
			printf("compute,");
			if(firstcompute < 0)firstcompute = j;
		}
		if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){	//1
			printf("graphic,");
			if(firstgraphic < 0)firstgraphic = j;
		}

		support = 0;
		if(surface){
			vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &support);
		}
		if(support){
			printf("present");
			if(firstpresent < 0)firstpresent = j;

			if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
				bestpresent = j;
			}
		}

		printf("\n");
	}
	printf("=>transfer@%d,compute@%d,graphic@%d,present@%d\n\n", firsttransfer, firstcompute, firstgraphic, firstpresent);

	if(surface){		//onscreen: need graphic and present
		if(bestpresent >= 0){
			if(gg)gg[0] = bestpresent;
			if(pp)pp[0] = bestpresent;
			return 1;
		}
		if((firstgraphic >= 0) && (firstpresent >= 0)){
			if(gg)gg[0] = firstgraphic;
			if(pp)pp[0] = firstpresent;
			return 1;
		}
	}
	else{		//offscreen: need graphic
		if(firstgraphic >= 0){
			if(gg)gg[0] = firstgraphic;
			return 1;
		}
	}
	if(0){		//need compute
		if(firstcompute > 0){
			return 1;
		}
	}

	return 0;
}
int checkSwapChain(VkPhysicalDevice device) {
	//format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, 0);
	if(0 == formatCount)return -1;

	VkSurfaceFormatKHR formats[formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats);

	int j;
	printf("vkGetPhysicalDeviceSurfaceFormatsKHR:\n");
	for(j=0;j<formatCount;j++){
		printf("%d:	format=%08x,colorspace=%08x\n", j, formats[j].format, formats[j].colorSpace);
	}
	printf("\n");

	//presentmode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes);

	printf("vkGetPhysicalDeviceSurfacePresentModesKHR:\n");
	for(j=0;j<formatCount;j++){
		printf("%d:	%08x=", j, presentModes[j]);
		switch(presentModes[j]){
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			printf("IMMEDIATE");
			break;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			printf("MAILBOX");
			break;
		case VK_PRESENT_MODE_FIFO_KHR:
			printf("FIFO");
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			printf("FIFO_RELAXED");
			break;
		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
			printf("SHARED_DEMAND_REFRESH");
			break;
		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
			printf("SHARED_CONTINUOUS_REFRESH");
			break;
		default:
			printf("unknown");
			break;
		}
		printf("\n");
	}
	printf("\n");

	return 1;
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
	int chkdev,chkext,chkfam,chksur;
	physicaldevice = VK_NULL_HANDLE;
	for(j=0;j<count;j++) {
		printf("%d:physicaldevice{\n", j);
		chkdev = checkDeviceProperties(devs[j]);
		chkext = checkDeviceExtensionProperties(devs[j]);
		chkfam = checkPhysicalDeviceQueueFamilyProperties(devs[j], 0, 0);
		if( (chkdev > 0) && (chkext > 0) && (chkfam > 0) && (phy < 0) ){
			if(surface){
				chksur = checkSwapChain(devs[j]);
				if(chksur > 0)phy = j;
			}
			else{
				phy = j;
			}
		}
		printf("score=%d,%d,%d,%d\n",chkdev,chkext,chkfam,chksur);
		printf("}\n\n");
	}
	if(phy < 0){
		printf("no physicaldevice\n");
		return -2;
	}

	physicaldevice = devs[phy];
	printf("=>choose device=%d(%p)\n", phy, physicaldevice);
	return 0;
}




int freelogicaldevice() {
	return 0;
}
int initlogicaldevice() {
	checkPhysicalDeviceQueueFamilyProperties(physicaldevice, &graphicindex, &presentindex);
	printf("graphic=%d,present=%d\n",graphicindex,presentindex);


	//device feature
	VkPhysicalDeviceFeatures deviceFeatures = {};


	//queue create info
	VkDeviceQueueCreateInfo queueCreateInfos[2] = {};
	float queuePriority = 1.0f;

	queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[0].queueFamilyIndex = graphicindex;
	queueCreateInfos[0].queueCount = 1;
	queueCreateInfos[0].pQueuePriorities = &queuePriority;

	queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[1].queueFamilyIndex = presentindex;
	queueCreateInfos[1].queueCount = 1;
	queueCreateInfos[1].pQueuePriorities = &queuePriority;


	//devicecreateinfo
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = surface ? 2 : 1;
	createInfo.pQueueCreateInfos = queueCreateInfos;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = surface ? 1 : 0;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.enabledLayerCount = 0;
	if (vkCreateDevice(physicaldevice, &createInfo, 0, &logicaldevice) != VK_SUCCESS) {
		printf("error@vkCreateDevice\n");
		return -1;
	}

	//graphic: queue, pool
	vkGetDeviceQueue(logicaldevice, graphicindex, 0, &graphicQueue);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicindex;
	if (vkCreateCommandPool(logicaldevice, &poolInfo, 0, &graphicPool) != VK_SUCCESS) {
		printf("error@vkCreateCommandPool\n");
	}


	//present: queue
	if(surface){
		vkGetDeviceQueue(logicaldevice, presentindex, 0, &presentQueue);
	}
	return 0;
}




int freeswapchain() {
	return 0;
}
int initswapchain() {
	//capability
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, surface, &capabilities);

	if(capabilities.currentExtent.width != UINT32_MAX) {
		widthheight.width = capabilities.currentExtent.width;
		widthheight.height= capabilities.currentExtent.height;
	}
	else{
		int w = 1024, h = 768;
		if(w > capabilities.maxImageExtent.width)w = capabilities.maxImageExtent.width;
		if(w < capabilities.minImageExtent.width)w = capabilities.minImageExtent.width;
		if(h > capabilities.maxImageExtent.height)h = capabilities.maxImageExtent.height;
		if(h < capabilities.minImageExtent.height)h = capabilities.minImageExtent.height;

		widthheight.width = capabilities.currentExtent.width;
		widthheight.height= capabilities.currentExtent.height;
	}

	uint32_t imageCount = capabilities.minImageCount + 1;
	if((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
		imageCount = capabilities.maxImageCount;
	}

	//format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, 0);
	if(0 == formatCount)return -1;

	VkSurfaceFormatKHR formats[formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, formats);

	int j;
	VkSurfaceFormatKHR surfaceFormat = formats[0];
	for(j=0;j<formatCount;j++){
		if(	(formats[j].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) ){
			surfaceFormat = formats[j];
		}
	}

	//presentmode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, presentModes);

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for(j=0;j<presentModeCount;j++){
		if(presentModes[j] == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = presentModes[j];
		}
	}

	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = widthheight;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {graphicindex, presentindex};
	if (graphicindex != presentindex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicaldevice, &createInfo, 0, &swapChain) != VK_SUCCESS) {
		printf("error@vkCreateSwapchainKHR\n");
		return -1;
	}

	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &imagecount, 0);
	VkImage swapChainImages[imagecount];
	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &imagecount, swapChainImages);

	for(j=0;j<imagecount; j++) {
		attachcolor[j].format = surfaceFormat.format;
		attachcolor[j].image = swapChainImages[j];
		attachcolor[j].memory = 0;

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[j];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = attachcolor[j].format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicaldevice, &createInfo, 0, &attachcolor[j].view) != VK_SUCCESS) {
			printf("error@vkCreateImageView:%d\n",j);
		}
	}
	return 0;
}




int freeoffscreen() {
	return 0;
}
int initoffscreen() {
	swapChain = 0;

	widthheight.width = 0;
	widthheight.height= 0;

	imagecount = 0;

	attachcolor[0].format = 0;
	return 0;
}




int vulkan_device_delete()
{
	if(surface)freeswapchain();
	else freeoffscreen();

	freelogicaldevice();
	freephysicaldevice();

	vkDestroySurfaceKHR(instance, surface, 0);
	return 0;
}
int vulkan_device_create(int forwhat, VkSurfaceKHR face)
{
	surface = face;

	//logicaldevice <- physicaldevice
	initphysicaldevice();
	initlogicaldevice();

	//swapchain <- physicaldevice, logicaldevice, surface
	if(surface)initswapchain();
	else initoffscreen();
	printf("swapchain imagecount=%d\n", imagecount);

	return 0;
}




void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev)
{
	*pdev = physicaldevice;
	*ldev = logicaldevice;
}
void vulkan_computequeue_computepool(VkQueue* queue, VkCommandPool* pool)
{
}
void vulkan_graphicqueue_graphicpool(VkQueue* queue, VkCommandPool* pool)
{
	*queue = graphicQueue;
	*pool = graphicPool;
}
void vulkan_presentqueue_presentpool(VkQueue* queue, VkCommandPool* pool)
{
	*queue = presentQueue;
}
void vulkan_swapchain_widthheight_imagecount_attachcolor(VkSwapchainKHR* chain, VkExtent2D* wh, uint32_t* cnt, struct attachment* attach)
{
	*chain = swapChain;

	wh->width = widthheight.width;
	wh->height = widthheight.height;

	*cnt = imagecount;

	int j;
	for(j=0;j<imagecount;j++){
		attach[j].format = attachcolor[j].format;
		attach[j].image = attachcolor[j].image;
		attach[j].view = attachcolor[j].view;
	}
}
