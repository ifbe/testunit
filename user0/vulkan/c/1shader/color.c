#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};




//0
static VkInstance instance;
//2
VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
VkDevice logicaldevice;
//3
int graphicindex;
int presentindex;
VkQueue graphicQueue;
VkQueue presentQueue;
VkCommandPool commandPool;
VkCommandBuffer commandBuffers[8];
//6
VkSurfaceKHR surface = 0;
VkSwapchainKHR swapChain;
VkExtent2D widthheight;
VkFormat swapChainImageFormat;
uint32_t swapChainImageCount;
VkImageView swapChainImageViews[8];
//7
VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;
VkFramebuffer framebuffer[8];
//
#define MAX_FRAMES_IN_FLIGHT 2
VkSemaphore imageAvailableSemaphores[2];
VkSemaphore renderFinishedSemaphores[2];
VkFence inFlightFences[2];
size_t currentFrame = 0;




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
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		printf("dgpu\n");
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
	return 1;
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
			printf("binding,");
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
			printf("graphic");
			if(firstgraphic < 0)firstgraphic = j;

			support = 0;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &support);
			if(support){
				printf("&present");
				if(firstpresent < 0)firstpresent = j;
			}
		}
		printf("\n");
	}
	printf("=>transfer@%d,compute@%d,graphic@%d,present@%d\n\n", firsttransfer, firstcompute, firstgraphic, firstpresent);

	if((firstgraphic >= 0) && (firstpresent >= 0)){
		if(gg)gg[0] = firstgraphic;
		if(pp)pp[0] = firstpresent;
		return 1;
	}
	return 0;
}
int checkSwapChain(VkPhysicalDevice device) {
	//capability
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

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
	int aa,bb,cc;
	physicaldevice = VK_NULL_HANDLE;
	for(j=0;j<count;j++) {
		printf("%d:physicaldevice{\n", j);
		checkDeviceProperties(devs[j]);
		aa = checkDeviceExtensionProperties(devs[j]);
		bb = checkPhysicalDeviceQueueFamilyProperties(devs[j], 0, 0);
		cc = checkSwapChain(devs[j]);

		if((aa >= 0)&&(bb > 0)&&(cc > 0)){
			if(phy < 0)phy = j;
		}
		printf("score=%d,%d,%d\n",aa,bb,cc);
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


	//device feature
	VkPhysicalDeviceFeatures deviceFeatures = {};


	//devicecreateinfo
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 2;
	createInfo.pQueueCreateInfos = queueCreateInfos;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 1;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.enabledLayerCount = 0;
	if (vkCreateDevice(physicaldevice, &createInfo, 0, &logicaldevice) != VK_SUCCESS) {
		printf("error@vkCreateDevice\n");
		return -1;
	}

	//queue
	vkGetDeviceQueue(logicaldevice, graphicindex, 0, &graphicQueue);
	vkGetDeviceQueue(logicaldevice, presentindex, 0, &presentQueue);

	//pool
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicindex;
	if (vkCreateCommandPool(logicaldevice, &poolInfo, 0, &commandPool) != VK_SUCCESS) {
		printf("error@vkCreateCommandPool\n");
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

	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &swapChainImageCount, 0);
	VkImage swapChainImages[swapChainImageCount];
	vkGetSwapchainImagesKHR(logicaldevice, swapChain, &swapChainImageCount, swapChainImages);

	swapChainImageFormat = surfaceFormat.format;
	for(j=0;j<swapChainImageCount; j++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[j];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicaldevice, &createInfo, 0, &swapChainImageViews[j]) != VK_SUCCESS) {
			printf("error@vkCreateImageView:%d\n",j);
		}
	}
	return 0;
}




int freeoffscreen() {
	return 0;
}
int initoffscreen() {
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
int vulkan_device_create(VkSurfaceKHR p)
{
	surface = p;

	//logicaldevice <- physicaldevice
	initphysicaldevice();
	initlogicaldevice();

	//swapchain <- physicaldevice, logicaldevice, surface
	if(surface)initswapchain();
	else initoffscreen();

	return 0;
}




int freerenderpass(){
	return 0;
}
int initrenderpass(){
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicaldevice, &renderPassInfo, 0, &renderPass) != VK_SUCCESS) {
		printf("error@vkCreateRenderPass\n");
	}
	return 0;
}




int createShaderModule(VkShaderModule* mod, char* url) {
	FILE* fp = fopen(url, "rb+");
	if(!fp)return -1;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	if(size <= 0){fclose(fp);return -2;}

	uint32_t* code = (uint32_t*)malloc(size);
	if(0 == code){fclose(fp);return -3;}

	fseek(fp, 0, SEEK_SET);
	if(fread(code,size,1,fp) < 1){fclose(fp);return -4;}
	fclose(fp);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = code;

	if (vkCreateShaderModule(logicaldevice, &createInfo, 0, mod) != VK_SUCCESS) {
		printf("error@vkCreateShaderModule\n");
		return -1;
	}
	return 0;
}
int freepipeline() {
	return 0;
}
int initpipeline() {
	//----------------shader----------------
	int ret;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	ret = createShaderModule(&vertShaderModule, "shader.vert.spv");
	if(ret < 0){
		printf("error:%d@createShaderModule:vert\n",ret);
		return -1;
	}
	ret = createShaderModule(&fragShaderModule, "shader.frag.spv");
	if(ret < 0){
		printf("error:%d@createShaderModule:frag\n",ret);
		return -2;
	}

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	//----------------vertex layout----------------
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	//----------------viewport, scissor----------------
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)widthheight.width;
	viewport.height = (float)widthheight.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = widthheight;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;


	//----------------rasterizer----------------
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;	//BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;


	//----------------multisample----------------
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


	//----------------blend----------------
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	//----------------pipeline----------------
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	if(vkCreatePipelineLayout(logicaldevice, &pipelineLayoutInfo, 0, &pipelineLayout) != VK_SUCCESS) {
		printf("error@vkCreatePipelineLayout\n");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	if (vkCreateGraphicsPipelines(logicaldevice, VK_NULL_HANDLE, 1, &pipelineInfo, 0, &graphicsPipeline) != VK_SUCCESS) {
		printf("error@vkCreateGraphicsPipelines\n");
	}


	//cleanup
	vkDestroyShaderModule(logicaldevice, fragShaderModule, 0);
	vkDestroyShaderModule(logicaldevice, vertShaderModule, 0);
	return 0;
}



int freeframebuffer(){
	return 0;
}
int initframebuffer(){
	for (size_t i = 0; i <swapChainImageCount; i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = widthheight.width;
		framebufferInfo.height = widthheight.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicaldevice, &framebufferInfo, 0, &framebuffer[i]) != VK_SUCCESS) {
			printf("error@vkCreateFramebuffer\n");
		}
	}
	return 0;
}




int freecommandbuffer(){
	return 0;
}
int initcommandbuffer() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapChainImageCount;

	if (vkAllocateCommandBuffers(logicaldevice, &allocInfo, commandBuffers) != VK_SUCCESS) {
		printf("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < swapChainImageCount; i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			printf("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffer[i];
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = widthheight;

		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		vkCmdDraw(commandBuffers[i], 6, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			printf("error@vkEndCommandBuffern");
		}
	}
	return 0;
}




int freesyncobject(){
	return 0;
}
int initsyncobject(){
	printf("color count=%d\n", swapChainImageCount);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if(vkCreateSemaphore(logicaldevice, &semaphoreInfo, 0, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
		vkCreateSemaphore(logicaldevice, &semaphoreInfo, 0, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
		vkCreateFence(logicaldevice, &fenceInfo, 0, &inFlightFences[i]) != VK_SUCCESS) {
			printf("failed to create synchronization objects for a frame\n");
		}
	}
	return 0;
}




void vulkan_myctx_delete()
{
	vkDeviceWaitIdle(logicaldevice);

	freesyncobject();

	freecommandbuffer();
	freeframebuffer();
	freepipeline();
	freerenderpass();
}
void vulkan_myctx_create()
{
	//pipeline <- renderpass
	//framebuffer <- imageview, renderpass
	//commandbuffer <- renderpass, pipeline, framebuffer, vertex
	initrenderpass();
	initpipeline();
	initframebuffer();
	initcommandbuffer();

	//fence
	initsyncobject();
}




void drawframe() {
	vkWaitForFences(logicaldevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicaldevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);


	//vkQueueSubmit
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicaldevice, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(graphicQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		printf("failed to submit draw command buffer\n");
	}


	//vkQueuePresentKHR
	if(surface){
		VkSwapchainKHR swapChains[] = {swapChain};
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		vkQueuePresentKHR(presentQueue, &presentInfo);
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
