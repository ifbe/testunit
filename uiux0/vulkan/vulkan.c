#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
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
	//createInfo.ppEnabledLayerNames = validationLayers;
	createInfo.pNext = 0;

	if (vkCreateInstance(&createInfo, 0, &instance) != VK_SUCCESS) {
		printf("failed to create instance!\n");
		return 0;
	}
	printf("instance=%p\n", instance);

/*
	VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
	debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debugReportCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debugMessageCallback;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
	VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(instance, &debugReportCreateInfo, nullptr, &debugReportCallback));
*/
	printf("----------------instance ok----------------\n\n");
	return instance;
}
void* vulkan_surface_create()
{
	return 0;
}
void vulkan_surface_delete(VkSurfaceKHR face)
{
	vkDestroySurfaceKHR(instance, face, 0);
}




int checkDeviceProperties(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(device, &prop);

	int score = -1;
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
		score = 1;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		printf("dgpu\n");
		score = 2;
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
	return score;
}
int checkDeviceExtensionProperties(VkPhysicalDevice device, void* name) {
	uint32_t cnt;
	vkEnumerateDeviceExtensionProperties(device, 0, &cnt, 0);

	VkExtensionProperties ext[cnt];
	vkEnumerateDeviceExtensionProperties(device, 0, &cnt, ext);
	printf("vkEnumerateDeviceExtensionProperties:\n");

	int j;
	int ret = -1;
	for(j=0;j<cnt;j++) {
		printf("%d:	ver=%04d,str=%s\n", j, ext[j].specVersion, ext[j].extensionName);
		if(0 == strcmp(ext[j].extensionName, name)){
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
int checkPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, VkSurfaceKHR face, int what){
	//printf("dev=%p\n",device);

	uint32_t cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, 0);

	VkQueueFamilyProperties fam[cnt];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, fam);

	int j;
	int firstgraphicwithpresent = -1;
	int firsttransfer = -1;
	int firstcompute = -1;
	int firstgraphic = -1;
	int firstpresent = -1;
	VkBool32 supportsurface = 0;
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

		if(face){
			supportsurface = 0;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, j, face, &supportsurface);

			if(supportsurface){
				printf("present");
				if(firstpresent < 0)firstpresent = j;

				if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
					if(firstgraphicwithpresent < 0)firstgraphicwithpresent = j;
				}
			}
		}

		printf("\n");
	}

	if(face){
		if( (firstgraphicwithpresent < 0) && (firstpresent < 0) )return -1;
	}
	if(what & VK_QUEUE_GRAPHICS_BIT){
		if(firstgraphic < 0)return -1;
	}
	if(what & VK_QUEUE_COMPUTE_BIT){
		if(firstcompute < 0)return -1;
	}
	return 1;
}
int checkSwapChain(VkPhysicalDevice device, VkSurfaceKHR face) {
	//format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, face, &formatCount, 0);
	if(0 == formatCount)return -1;

	VkSurfaceFormatKHR formats[formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, face, &formatCount, formats);

	int j;
	printf("vkGetPhysicalDeviceSurfaceFormatsKHR:\n");
	for(j=0;j<formatCount;j++){
		printf("%d:	format=%08x,colorspace=%08x\n", j, formats[j].format, formats[j].colorSpace);
	}
	printf("\n");

	//presentmode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, face, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, face, &presentModeCount, presentModes);

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
void* initphysicaldevice(int what, VkSurfaceKHR face) {
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, 0);
	if(0 == count) {
		printf("error@vkEnumeratePhysicalDevices\n");
		return 0;
	}

	VkPhysicalDevice devs[count];
	vkEnumeratePhysicalDevices(instance, &count, devs);
	printf("vkEnumeratePhysicalDevices:\n");

	int j,best=-1;
	int chkdev,chkext,chkfam,chksur;
	physicaldevice = VK_NULL_HANDLE;
	for(j=0;j<count;j++) {
		printf("%d:physicaldevice{\n", j);
		chkdev = checkDeviceProperties(devs[j]);
		chkext = checkDeviceExtensionProperties(devs[j], VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		chkfam = checkPhysicalDeviceQueueFamilyProperties(devs[j], face, what);
		if( (chkdev > 0) && (chkext > 0) && (chkfam > 0) && (best < 0) ){
			if(face){
				chksur = checkSwapChain(devs[j], face);
				if(chksur > 0)best = j;
			}
			else{
				best = j;
			}
		}
		printf("score=%d,%d,%d,%d\n",chkdev,chkext,chkfam,chksur);
		printf("}\n\n");
	}
	if(best < 0){
		printf("no physicaldevice\n");
		return 0;
	}

	printf("=>choose device=%d(%p)\n", best, devs[best]);
	return devs[best];
}




int getPhysicalDeviceQueueFamilyProperties(
	VkPhysicalDevice device, VkSurfaceKHR face,
	int* gg, int* pp, int* cc)
{
	//printf("dev=%p\n",device);

	uint32_t cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, 0);

	VkQueueFamilyProperties fam[cnt];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &cnt, fam);

	int j;
	int firstgraphicwithpresent = -1;
	int firsttransfer = -1;
	int firstcompute = -1;
	int firstgraphic = -1;
	int firstpresent = -1;
	VkBool32 supportsurface = 0;
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

		supportsurface = 0;
		if(face){
			vkGetPhysicalDeviceSurfaceSupportKHR(device, j, face, &supportsurface);
		}
		if(supportsurface){
			printf("present");
			if(firstpresent < 0)firstpresent = j;

			if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
				if(firstgraphicwithpresent < 0)firstgraphicwithpresent = j;
			}
		}

		printf("\n");
	}

	if(face){		//onscreen: need graphic and present
		if(firstgraphicwithpresent >= 0){
			if(gg)gg[0] = firstgraphicwithpresent;
			if(pp)pp[0] = firstgraphicwithpresent;
		}
		if((firstgraphic >= 0) && (firstpresent >= 0)){
			if(gg)gg[0] = firstgraphic;
			if(pp)pp[0] = firstpresent;
		}
	}
	else{		//offscreen: need graphic
		if(firstgraphic >= 0){
			if(gg)gg[0] = firstgraphic;
		}
	}
	if(firstcompute > 0){
		if(cc)cc[0] = firstcompute;
	}

	return 0;
}
int freelogicaldevice() {
	return 0;
}
int initlogicaldevice(int what, VkSurfaceKHR face) {
	//queue index
	getPhysicalDeviceQueueFamilyProperties(physicaldevice, face, &graphicindex, &presentindex, &computeindex);
	printf("graphic,present=%d,%d, compute=%d\n",graphicindex,presentindex, computeindex);

	//queue create info
	VkDeviceQueueCreateInfo queueCreateInfos[3] = {};
	float queuePriority = 1.0f;
	int queuecount = 0;
	printf("queueCreateInfos:");
	if((what&VK_QUEUE_GRAPHICS_BIT) && (graphicindex >= 0)){
		printf("%d=graphic,", queuecount);
		queueCreateInfos[queuecount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[queuecount].queueFamilyIndex = graphicindex;
		queueCreateInfos[queuecount].queueCount = 1;
		queueCreateInfos[queuecount].pQueuePriorities = &queuePriority;
		queuecount += 1;
	}
	if((what&VK_QUEUE_COMPUTE_BIT) && (computeindex >= 0)){
		printf("%d=compute,", queuecount);
		queueCreateInfos[queuecount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[queuecount].queueFamilyIndex = presentindex;
		queueCreateInfos[queuecount].queueCount = 1;
		queueCreateInfos[queuecount].pQueuePriorities = &queuePriority;
		queuecount += 1;
	}
	if(face && (presentindex >= 0) && (graphicindex != presentindex)){
		printf("%d=present,", queuecount);
		queueCreateInfos[queuecount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[queuecount].queueFamilyIndex = presentindex;
		queueCreateInfos[queuecount].queueCount = 1;
		queueCreateInfos[queuecount].pQueuePriorities = &queuePriority;
		queuecount += 1;
	}
	printf("\n");


	//device feature
	VkPhysicalDeviceFeatures deviceFeatures = {};

	//device extension
	const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	//devicecreateinfo
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queuecount;
	createInfo.pQueueCreateInfos = queueCreateInfos;
	createInfo.enabledExtensionCount = face ? 1 : 0;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledLayerCount = 0;
	if (vkCreateDevice(physicaldevice, &createInfo, 0, &logicaldevice) != VK_SUCCESS) {
		printf("error@vkCreateDevice\n");
		return -1;
	}
	printf("logicaldevice=%p\n", logicaldevice);

	//graphic: queue, pool
	if((what&VK_QUEUE_GRAPHICS_BIT) && (graphicindex >= 0)){
		vkGetDeviceQueue(logicaldevice, graphicindex, 0, &graphicQueue);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicindex;
		if (vkCreateCommandPool(logicaldevice, &poolInfo, 0, &graphicPool) != VK_SUCCESS) {
			printf("error@vkCreateCommandPool\n");
		}
		printf("graphicpool=%p\n",graphicPool);
	}

	//compute: queue, pool
	if((what&VK_QUEUE_COMPUTE_BIT) && (computeindex >= 0)){
		vkGetDeviceQueue(logicaldevice, computeindex, 0, &computeQueue);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = computeindex;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if (vkCreateCommandPool(logicaldevice, &poolInfo, 0, &computePool) != VK_SUCCESS) {
			printf("error@vkCreateCommandPool\n");
		}
		printf("computepool=%p\n",computePool);
	}

	//present: queue
	if(face){
		vkGetDeviceQueue(logicaldevice, presentindex, 0, &presentQueue);
	}
	return 0;
}




int freeswapchain() {
	return 0;
}
int initswapchain(VkSurfaceKHR face) {
	//capability
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, face, &capabilities);

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
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, face, &formatCount, 0);
	if(0 == formatCount)return -1;

	VkSurfaceFormatKHR formats[formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, face, &formatCount, formats);

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
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, face, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, face, &presentModeCount, presentModes);

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for(j=0;j<presentModeCount;j++){
		if(presentModes[j] == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = presentModes[j];
		}
	}

	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = face;

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
	printf("swapchain=%p\n",swapChain);


	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &imagecount, 0);
	VkImage swapChainImages[imagecount];
	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &imagecount, swapChainImages);

	printf("swapchain imagecount=%d\n", imagecount);
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
		printf("%d:image=%p,view=%p\n", j, swapChainImages[j], attachcolor[j].view);
	}
	return 0;
}




int vulkan_device_delete()
{
	if(surface){
		freeswapchain();
	}

	freelogicaldevice();

	freephysicaldevice();

	return 0;
}
void* vulkan_device_create(int what, VkSurfaceKHR face)
{
	//logicaldevice <- physicaldevice
	physicaldevice = initphysicaldevice(what, face);
	if(0 == physicaldevice)return 0;
	printf("----------------physicaldevice ok----------------\n\n");

	initlogicaldevice(what, face);
	if(0 == logicaldevice)return 0;
	printf("----------------logicaldevice and queue ok----------------\n\n");

	//swapchain <- physicaldevice, logicaldevice, surface
	if(face){
		initswapchain(face);
		surface = face;
		printf("----------------swapchain and image ok----------------\n\n");
	}

	return physicaldevice;
}




void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev)
{
	*pdev = physicaldevice;
	*ldev = logicaldevice;
}
void vulkan_graphicqueue_graphicpool(VkQueue* queue, VkCommandPool* pool)
{
	*queue = graphicQueue;
	*pool = graphicPool;
}
void vulkan_computequeue_computepool(VkQueue* queue, VkCommandPool* pool)
{
	*queue = computeQueue;
	*pool = computePool;
}
void vulkan_presentqueue_swapchain(VkQueue* queue, void** chain)
{
	*queue = presentQueue;
	*chain = swapChain;
}
void vulkan_widthheight_imagecount_attachcolor(VkExtent2D* wh, uint32_t* cnt, struct attachment* attach)
{
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