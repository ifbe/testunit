#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
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
static void* glfw;	//for fun
static VkInstance instance;
//1
static GLFWwindow* window = 0;
VkSurfaceKHR surface;
//2
VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
VkDevice logicaldevice;
//3
VkQueue graphicQueue;
VkQueue presentQueue;
VkCommandPool commandPool;
//6
VkSwapchainKHR swapChain;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;
uint32_t swapChainImageCount;
VkImageView swapChainImageViews[256];
//7
VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;
VkFramebuffer swapChainFramebuffers[256];
VkCommandBuffer commandBuffers[256];
//
#define MAX_FRAMES_IN_FLIGHT 2
VkSemaphore imageAvailableSemaphores[2];
VkSemaphore renderFinishedSemaphores[2];
VkFence inFlightFences[2];
VkFence imagesInFlight[256];
size_t currentFrame = 0;







int freeglfw()
{
	glfwTerminate();
	return 0;
}
int initglfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return 0;
}




int freeinstance()
{
	vkDestroyInstance(instance, 0);
	return 0;
}
int initinstance()
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




	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t count = 0;
	const char** extension = glfwGetRequiredInstanceExtensions(&count);
	printf("glfwGetRequiredInstanceExtensions:\n");

	for(j=0;j<count;j++){
		printf("%4d:%s\n", j, extension[j]);
	}

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = count;
	createInfo.ppEnabledExtensionNames = extension;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = 0;

	if (vkCreateInstance(&createInfo, 0, &instance) != VK_SUCCESS) {
		printf("failed to create instance!");
	}	
	return 0;
}




int freeglfwwindow()
{
	glfwDestroyWindow(window);
	return 0;
}
int initglfwwindow()
{
	window = glfwCreateWindow(1024, 768, "Vulkan window", 0, 0);
	if(0 == window){
		printf("error@glfwCreateWindow\n");
		return -1;
	}
	return 0;
}




int freesurface()
{
	vkDestroySurfaceKHR(instance, surface, 0);
	return 0;
}
int initsurface()
{
	if (glfwCreateWindowSurface(instance, window, 0, &surface) != VK_SUCCESS) {
		printf("error@glfwCreateWindowSurface\n");
		return -1;
	}
	return 0;
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
		if(fam[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			graphicSupport = 1;
			graphicwhich = j;
		}

		presentSupport = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &presentSupport);
		if(presentSupport){
			presentwhich = j;
		}
	}

	if(gg)gg[0] = graphicwhich;
	if(pp)pp[0] = presentwhich;
	return (graphicSupport >= 0) && (presentSupport >= 0);
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
	printf("vkGetPhysicalDeviceSurfaceFormatsKHR:\n");

	int j;
	for(j=0;j<formatCount;j++){
		printf("%4d:format=%08x,colorspace=%08x\n", j, formats[j].format, formats[j].colorSpace);
	}

	//presentmode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes);
	printf("vkGetPhysicalDeviceSurfacePresentModesKHR:\n");

	for(j=0;j<formatCount;j++){
		printf("%4d:%08x\n", j, presentModes[j]);
	}

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
		printf("physicaldevice %d:\n", j);
		aa = checkDeviceExtensionProperties(devs[j]);
		bb = checkPhysicalDeviceQueueFamilyProperties(devs[j], 0, 0);
		cc = checkSwapChain(devs[j]);

		printf("%d,%d,%d\n",aa,bb,cc);
		if((aa >= 0)&&(bb >= 0)&&(cc > 0)){
			if(phy < 0)phy = j;
		}
	}
	if(phy < 0){
		printf("no physicaldevice\n");
		return -2;
	}

	physicaldevice = devs[phy];
	printf("physicaldevice=%d(%p)\n", phy, physicaldevice);
	return 0;
}




int freelogicaldevice() {
	return 0;
}
int initlogicaldevice() {
	int graphic, present;
	checkPhysicalDeviceQueueFamilyProperties(physicaldevice, &graphic, &present);
	printf("graphic=%d,present=%d\n",graphic,present);


	//queue create info
	VkDeviceQueueCreateInfo queueCreateInfos[2];
	float queuePriority = 1.0f;

	queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[0].queueFamilyIndex = graphic;
	queueCreateInfos[0].queueCount = 1;
	queueCreateInfos[0].pQueuePriorities = &queuePriority;

	queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[1].queueFamilyIndex = present;
	queueCreateInfos[1].queueCount = 1;
	queueCreateInfos[1].pQueuePriorities = &queuePriority;


	//device feature
	VkPhysicalDeviceFeatures deviceFeatures = {0};


	//devicecreateinfo
	VkDeviceCreateInfo createInfo;
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
	vkGetDeviceQueue(logicaldevice, graphic, 0, &graphicQueue);
	vkGetDeviceQueue(logicaldevice, present, 0, &presentQueue);

	//pool
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphic;
	if (vkCreateCommandPool(logicaldevice, &poolInfo, 0, &commandPool) != VK_SUCCESS) {
		printf("error@vkCreateCommandPool\n");
	}
	return 0;
}




int getmin(int a, int b)
{
	return a < b ? a : b;
}
int getmax(int a, int b)
{
	return a > b ? a : b;
}
int freeswapchain() {
	return 0;
}
int initswapchain() {
	//capability
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, surface, &capabilities);

	//format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, 0);
	if(0 == formatCount)return -1;

	VkSurfaceFormatKHR formats[formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, formats);

	//presentmode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, 0);
	if(0 == presentModeCount)return -2;

	VkPresentModeKHR presentModes[presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, presentModes);

	int j;
	VkSurfaceFormatKHR surfaceFormat = formats[0];
    for(j=0;j<formatCount;j++){
		if(	(formats[j].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) ){
		surfaceFormat = formats[j];
	}
    }

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(j=0;j<formatCount;j++){
		if(presentModes[j] == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = presentModes[j];
		}
	}

	VkExtent2D extent;
	if(capabilities.currentExtent.width != UINT32_MAX) {
		extent = capabilities.currentExtent;
	}
	else{
		VkExtent2D actualExtent = {1024, 768};
		actualExtent.width = getmax(capabilities.minImageExtent.width, getmin(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = getmax(capabilities.minImageExtent.height, getmin(capabilities.maxImageExtent.height, actualExtent.height));

		extent = actualExtent;
	}

	uint32_t imageCount = capabilities.minImageCount + 1;
	if((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
		imageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	int graphic, present;
	checkPhysicalDeviceQueueFamilyProperties(physicaldevice, &graphic, &present);

	uint32_t queueFamilyIndices[] = {graphic, present};
	if (graphic != present) {
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
	swapChainExtent = extent;
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
	viewport.width = (float) swapChainExtent.width;
	viewport.height = (float) swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapChainExtent;

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
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
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
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicaldevice, &framebufferInfo, 0, &swapChainFramebuffers[i]) != VK_SUCCESS) {
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
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

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
	printf("count=%d\n", swapChainImageCount);

	int j;
	for(j=0;j<swapChainImageCount;j++)imagesInFlight[j] = VK_NULL_HANDLE;

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




void drawFrame() {
	vkWaitForFences(logicaldevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicaldevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(logicaldevice, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicaldevice, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(graphicQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		printf("failed to submit draw command buffer\n");
	}

	VkSwapchainKHR swapChains[] = {swapChain};
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	vkQueuePresentKHR(presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
int main()
{
	//glfw, vulkan
	initglfw();
	initinstance();

	//window, surface
	initglfwwindow();
	initsurface();

	//logicaldevice <- physicaldevice
	//swapchain <- physicaldevice, logicaldevice, surface
	initphysicaldevice();
	initlogicaldevice();
	initswapchain();

	//pipeline <- renderpass
	//framebuffer <- imageview, renderpass
	//commandbuffer <- renderpass, pipeline, framebuffer, vertex
	initrenderpass();
	initpipeline();
	initframebuffer();
	initcommandbuffer();
	initsyncobject();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(logicaldevice);

	freesyncobject();
	freecommandbuffer();
	freeframebuffer();
	freepipeline();
	freerenderpass();
	freeswapchain();
	freelogicaldevice();
	freephysicaldevice();
	freesurface();
	freeglfwwindow();

	freeinstance();
	freeglfw();
	return 0;
}
