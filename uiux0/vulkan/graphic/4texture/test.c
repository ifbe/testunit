#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vulkan/vulkan.h>
struct attachment{
	VkFormat format;
	VkDeviceMemory memory;
	VkImage image;
	VkImageView view;
};
void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev);
void vulkan_graphicqueue_graphicpool(VkQueue* queue, VkCommandPool* pool);
void vulkan_presentqueue_presentpool(VkQueue* queue, VkCommandPool* pool);
void vulkan_swapchain_widthheight_imagecount_attachcolor(VkSwapchainKHR* chain, VkExtent2D* wh, uint32_t* cnt, struct attachment* attach);




VkPhysicalDevice physicaldevice;
VkDevice logicaldevice;
//
VkQueue graphicQueue;
VkCommandPool graphicPool;
VkQueue presentQueue;
VkCommandPool presentPool;		//not exist
//
VkSwapchainKHR swapChain = 0;
VkExtent2D widthheight;
uint32_t imagecount;
struct attachment attachcolor[8];
struct attachment attachdepth;
//command
VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkDescriptorSetLayout descsetLayout;
VkPipeline graphicsPipeline;
VkFramebuffer framebuffer[8];
VkCommandBuffer commandBuffers[8];
//5
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;
//
VkBuffer uniformBuffers[16];
VkDeviceMemory uniformBuffersMemory[16];
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSets[16];
//
VkImage textureImage;
VkDeviceMemory textureImageMemory;
VkImageView textureImageView;
VkSampler textureSampler;
//fence
#define MAX_FENCE_IN_FLIGHT 2
VkSemaphore imageAvailableSemaphores[MAX_FENCE_IN_FLIGHT];
VkSemaphore renderFinishedSemaphores[MAX_FENCE_IN_FLIGHT];
VkFence inFlightFences[MAX_FENCE_IN_FLIGHT];
size_t currentFrame = 0;
float time = 0.0;



#define VERTEXCOUNT 8
#define INDICECOUNT 12
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef float mat2[2][2];
typedef float mat3[3][3];
typedef float mat4[4][4];
struct Vertex {
    vec3 pos;
    vec3 color;
	vec2 texcoord;
};
struct Vertex vertices[] = {
	{{-0.25f,-0.25f,-0.25f}, {1.0f, 0.0f, 0.0f}, {0.0, 0.0}},
	{{ 0.50f,-0.25f,-0.25f}, {0.0f, 1.0f, 0.0f}, {1.0, 0.0}},
	{{ 0.50f, 0.25f, 0.25f}, {0.0f, 0.0f, 1.0f}, {1.0, 1.0}},
	{{-0.25f, 0.25f, 0.25f}, {1.0f, 1.0f, 1.0f}, {0.0, 1.0}},

	{{-0.50f,-0.25f, 0.25f}, {0.0f, 1.0f, 1.0f}, {0.0, 0.0}},
	{{ 0.25f,-0.25f, 0.25f}, {1.0f, 0.0f, 1.0f}, {1.0, 0.0}},
	{{ 0.25f, 0.25f,-0.25f}, {0.0f, 0.0f, 0.0f}, {1.0, 1.0}},
	{{-0.50f, 0.25f,-0.25f}, {1.0f, 1.0f, 1.0f}, {0.0, 1.0}}
};
uint16_t indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};
struct UniformBufferObject {
	mat4 wvp;
};




VkFormat findDepthFormat() {
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	VkFormat fmt[] = {
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT
	};

	int j;
	VkFormatProperties props;
	for(j=0;j<3;j++){
		vkGetPhysicalDeviceFormatProperties(physicaldevice, fmt[j], &props);
		if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)return fmt[j];
		if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)return fmt[j];
	}
	return 0;
}
int findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicaldevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}
int freedepthstencil() {
	return 0;
}
int initdepthstencil() {
	attachdepth.format = findDepthFormat();


	//image
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = widthheight.width;
	imageInfo.extent.height = widthheight.width;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = attachdepth.format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateImage(logicaldevice, &imageInfo, 0, &attachdepth.image) != VK_SUCCESS) {
		printf("error@vkCreateImage:attachdepth.image\n");
	}

	VkMemoryRequirements memreq;
	vkGetImageMemoryRequirements(logicaldevice, attachdepth.image, &memreq);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memreq.size;
	allocInfo.memoryTypeIndex = findMemoryType(memreq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(logicaldevice, &allocInfo, 0, &attachdepth.memory) != VK_SUCCESS) {
		printf("error@vkAllocateMemory:attachdepth.memory\n");
	}

	vkBindImageMemory(logicaldevice, attachdepth.image, attachdepth.memory, 0);


	//imageview
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = attachdepth.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = attachdepth.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(logicaldevice, &viewInfo, 0, &attachdepth.view) != VK_SUCCESS) {
		printf("error@vkCreateImageView:depth\n");
	}
	return 0;
}




int freerenderpass(){
	return 0;
}
int initrenderpass(){
	//color
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = attachcolor[0].format;
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


	//depth
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = attachdepth.format;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	//
	VkAttachmentDescription attachments[2] = {
		colorAttachment,
		depthAttachment
	};

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicaldevice, &renderPassInfo, 0, &renderPass) != VK_SUCCESS) {
		printf("error@vkCreateRenderPass\n");
	}
	return 0;
}




int freedescsetlayout() {
	return 0;
}
int initdescsetlayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = 0;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = 0;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] = {
		uboLayoutBinding,
		samplerLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = bindings;
	if (vkCreateDescriptorSetLayout(logicaldevice, &layoutInfo, 0, &descsetLayout) != VK_SUCCESS) {
		printf("error@vkCreateDescriptorSetLayout\n");
		return -1;
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
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = 4*(3+3+2);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attributeDescriptions[3] = {};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = 4*3;
	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = 4*6;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = 3;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	//----------------viewport, scissor----------------
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) widthheight.width;
	viewport.height = (float) widthheight.height;
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
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;


	//----------------multisample----------------
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


	//----------------depthstencil----------------
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;


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
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descsetLayout;
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
	pipelineInfo.pDepthStencilState = &depthStencil;
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
	for (size_t i = 0; i <imagecount; i++) {
		VkImageView attachments[] = {
			attachcolor[i].view,
			attachdepth.view
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
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




int freevertexbuffer(){
	vkDestroyBuffer(logicaldevice, vertexBuffer, 0);
	vkFreeMemory(logicaldevice, vertexBufferMemory, 0);
	return 0;
}
int initvertexbuffer(){
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = 4*(3+3+2)*VERTEXCOUNT;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicaldevice, &bufferInfo, 0, &vertexBuffer) != VK_SUCCESS) {
		printf("error@vkCreateBuffer:vertex\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicaldevice, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(logicaldevice, &allocInfo, 0, &vertexBufferMemory) != VK_SUCCESS) {
		printf("error@vkAllocateMemory:vertex\n");
	}

	vkBindBufferMemory(logicaldevice, vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(logicaldevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices, (size_t) bufferInfo.size);
	vkUnmapMemory(logicaldevice, vertexBufferMemory);

	return 0;
}
int freeindicebuffer(){
        vkDestroyBuffer(logicaldevice, indexBuffer, 0);
        vkFreeMemory(logicaldevice, indexBufferMemory, 0);
	return 0;
}
int initindicebuffer(){
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = 2*INDICECOUNT;
	bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicaldevice, &bufferInfo, 0, &indexBuffer) != VK_SUCCESS) {
		printf("error@vkCreateBuffer:indice\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicaldevice, indexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(logicaldevice, &allocInfo, 0, &indexBufferMemory) != VK_SUCCESS) {
		printf("error@vkAllocateMemory:indice\n");
	}

	vkBindBufferMemory(logicaldevice, indexBuffer, indexBufferMemory, 0);

	void* data;
	vkMapMemory(logicaldevice, indexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, indices, (size_t) bufferInfo.size);
	vkUnmapMemory(logicaldevice, indexBufferMemory);

	return 0;
}




int createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicaldevice, &bufferInfo, 0, buffer) != VK_SUCCESS) {
		printf("error@vkCreateBuffer\n");
		return -1;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicaldevice, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(logicaldevice, &allocInfo, 0, bufferMemory) != VK_SUCCESS) {
		printf("error@vkAllocateMemory\n");
		return -2;
	}

	vkBindBufferMemory(logicaldevice, *buffer, *bufferMemory, 0);
	return 0;
}
int freeuniformbuffers() {
	return 0;
}
int inituniformbuffers() {
	VkDeviceSize bufferSize = sizeof(struct UniformBufferObject);

	for (size_t i = 0; i < imagecount; i++) {
		createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&uniformBuffers[i],
			&uniformBuffersMemory[i]
		);
	}
	return 0;
}




int createImage(
	uint32_t width, uint32_t height,
	VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	VkImage* image, VkDeviceMemory* imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateImage(logicaldevice, &imageInfo, 0, image) != VK_SUCCESS) {
		printf("error@vkCreateImage\n");
		return -1;
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(logicaldevice, *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(logicaldevice, &allocInfo, 0, imageMemory) != VK_SUCCESS) {
		printf("error@vkAllocateMemory\n");
		return -2;
	}

	vkBindImageMemory(logicaldevice, *image, *imageMemory, 0);
	return 0;
}
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicaldevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);


	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		printf("unsupported layout transition!\n");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, 0,
		0, 0,
		1, &barrier
	);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicQueue);
	vkFreeCommandBuffers(logicaldevice, graphicPool, 1, &commandBuffer);
}
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicaldevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);


	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageOffset.z = 0;
	region.imageExtent.width = width;
	region.imageExtent.height= height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicQueue);
	vkFreeCommandBuffers(logicaldevice, graphicPool, 1, &commandBuffer);
}
int freetexture(){
	return 0;
}
int inittexture() {
	int texWidth = 512;
	int texHeight = 512;
	int texChannels = 4;
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	int x,y;
	unsigned char* rgba = (unsigned char*)malloc(512*512*4);
	for(y=0;y<texHeight;y++){
		for(x=0;x<texWidth;x++){
			rgba[4*(y*texWidth+x)+0] = ((x&0x3f) > 0x20) ? x/2 : 0;
			rgba[4*(y*texWidth+x)+1] = ((y&0x3f) > 0x20) ? y/2 : 0;
			rgba[4*(y*texWidth+x)+2] = ((x*y*x)+(y*x*y))&0xff;
			rgba[4*(y*texWidth+x)+3] = 0xff;
		}
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(logicaldevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, rgba, imageSize);
	vkUnmapMemory(logicaldevice, stagingBufferMemory);

	free(rgba);

	createImage(
		texWidth,
		texHeight,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&textureImage,
		&textureImageMemory
	);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(logicaldevice, stagingBuffer, 0);
	vkFreeMemory(logicaldevice, stagingBufferMemory, 0);




	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(logicaldevice, &viewInfo, 0, &textureImageView) != VK_SUCCESS) {
		printf("error@vkCreateImageView\n");
		return -2;
	}
	return 0;
}




int freesampler(){
	return 0;
}
int initsampler() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	if (vkCreateSampler(logicaldevice, &samplerInfo, 0, &textureSampler) != VK_SUCCESS) {
		printf("error@vkCreateSampler\n");
		return -1;
	}
	return 0;
}




int freedescriptor(){
	return 0;
}
int initdescriptor() {
	VkDescriptorPoolSize poolSizes[2] = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = imagecount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = imagecount;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = imagecount;

	if (vkCreateDescriptorPool(logicaldevice, &poolInfo, 0, &descriptorPool) != VK_SUCCESS) {
		printf("error@vkCreateDescriptorPool\n");
		return -1;
	}


	int j;
	VkDescriptorSetLayout layouts[imagecount];
	for(j=0;j<imagecount;j++)layouts[j] = descsetLayout;

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = imagecount;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(logicaldevice, &allocInfo, descriptorSets) != VK_SUCCESS) {
		printf("error@vkAllocateDescriptorSets\n");
		return -2;
	}

	for (size_t i = 0; i < imagecount; i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(struct UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		VkWriteDescriptorSet descriptorWrites[2] = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
		vkUpdateDescriptorSets(logicaldevice, 2, descriptorWrites, 0, 0);
	}
	return 0;
}




int freecommandbuffer(){
	return 0;
}
int initcommandbuffer() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = graphicPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = imagecount;
	if (vkAllocateCommandBuffers(logicaldevice, &allocInfo, commandBuffers) != VK_SUCCESS) {
		printf("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < imagecount; i++) {
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

		VkClearValue clearValues[2] = {};
		clearValues[0].color.float32[0] = 0.0;
		clearValues[0].color.float32[1] = 0.0;
		clearValues[0].color.float32[2] = 0.0;
		clearValues[0].color.float32[3] = 1.0;
		clearValues[1].depthStencil.depth = 1.0f;
		clearValues[1].depthStencil.stencil = 0;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, 0);
        vkCmdDrawIndexed(commandBuffers[i], 2*INDICECOUNT, 1, 0, 0, 0);

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
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FENCE_IN_FLIGHT; i++) {
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
	freedescriptor();

	freesampler();
	freetexture();
	freeuniformbuffers();
	freeindicebuffer();
	freevertexbuffer();

	freeframebuffer();
	freepipeline();
	freedescsetlayout();
	freerenderpass();

	freedepthstencil();
}
void vulkan_myctx_create()
{
	vulkan_physicaldevice_logicdevice(&physicaldevice, &logicaldevice);
	vulkan_graphicqueue_graphicpool(&graphicQueue, &graphicPool);
	vulkan_presentqueue_presentpool(&presentQueue, 0);
	vulkan_swapchain_widthheight_imagecount_attachcolor(&swapChain, &widthheight, &imagecount, attachcolor);

	initdepthstencil();

	//pipeline <- renderpass, descsetlayout
	//framebuffer <- imageview, renderpass
	//commandbuffer <- renderpass, pipeline, framebuffer, vertex, indice
	initrenderpass();
	initdescsetlayout();
	initpipeline();
	initframebuffer();

	initvertexbuffer();
	initindicebuffer();
	inituniformbuffers();
	inittexture();
	initsampler();

	//...
	initdescriptor();
	initcommandbuffer();
	initsyncobject();
}




void updateUniformBuffer(uint32_t currentImage) {
/*	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
*/
	float c = cos(time);
	float s = sin(time);
	time += 0.01;

	struct UniformBufferObject ubo;
	ubo.wvp[0][0] = 1.0;ubo.wvp[0][1] = 0.0;ubo.wvp[0][2] = 0.0;ubo.wvp[0][3] = 0.0;
	ubo.wvp[1][0] = 0.0;ubo.wvp[1][1] =   c;ubo.wvp[1][2] =  -s;ubo.wvp[1][3] = 0.0;
	ubo.wvp[2][0] = 0.0;ubo.wvp[2][1] =   s;ubo.wvp[2][2] =   c;ubo.wvp[2][3] = 0.0;
	ubo.wvp[3][0] = 0.0;ubo.wvp[3][1] = 0.0;ubo.wvp[3][2] = 0.5;ubo.wvp[3][3] = 1.0;

	void* data;
	vkMapMemory(logicaldevice, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(logicaldevice, uniformBuffersMemory[currentImage]);
}
void drawframe() {
	uint32_t imageIndex;
	if(swapChain){		//onscreen: acquire next image
		vkAcquireNextImageKHR(logicaldevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	}
	else{		//offscreen: get next image

	}

	updateUniformBuffer(imageIndex);


	//vkQueueSubmit
	if(VK_SUCCESS != vkWaitForFences(logicaldevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX) ){
		printf("fail@vkWaitForFences\n");
	}

	if(VK_SUCCESS != vkResetFences(logicaldevice, 1, &inFlightFences[currentFrame]) ){
		printf("fail@vkResetFences\n");
	}

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

	currentFrame = (currentFrame + 1) % MAX_FENCE_IN_FLIGHT;


	//offscreen or onscreen
	if(swapChain){	//onscreen: vkQueuePresentKHR
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
	else{		//offscreen: copy to image
	}
}