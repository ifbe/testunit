#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
void* vulkan_device_create(int, void*);
void vulkan_device_delete(void*);
void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev);
void vulkan_computequeue_computepool(VkQueue* queue, VkCommandPool* pool);




VkPhysicalDevice physicaldevice = 0;
VkDevice logicaldevice = 0;
//
VkQueue computeQueue;
VkCommandPool computePool;
//
VkShaderModule shaderModule;
VkCommandPool commandPool;
VkCommandBuffer commandBuffer;
VkFence computefence;
//
VkDescriptorPool descriptorPool;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorSet descriptorSet;
//
VkPipelineLayout pipelineLayout;
VkPipelineCache pipelineCache;
VkPipeline computepipeline;
//
VkBuffer hostBuffer;
VkDeviceMemory hostMemory;
VkBuffer deviceBuffer;
VkDeviceMemory deviceMemory;
//
int bufferSize = 4*1024*1024*256;





int findMemoryType(VkPhysicalDevice pdev, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(pdev, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}
VkResult createBuffer(
	VkBuffer* buffer, VkDeviceMemory* memory,
	VkPhysicalDevice pdev, VkDevice ldev,
	VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
	int size, void *data)
{
	//buffer handle
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkCreateBuffer(ldev, &bufferInfo, 0, buffer);


	//buffer memory
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(ldev, *buffer, &memReqs);

	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(pdev, &deviceMemoryProperties);

	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.allocationSize = memReqs.size;
	memAlloc.memoryTypeIndex = findMemoryType(pdev, memReqs.memoryTypeBits, memoryPropertyFlags);
	vkAllocateMemory(ldev, &memAlloc, 0, memory);


	//copy
	if (data) {
		void *mapped = 0;
		vkMapMemory(ldev, *memory, 0, size, 0, &mapped);
		memcpy(mapped, data, size);
		vkUnmapMemory(ldev, *memory);
	}


	//bind
	vkBindBufferMemory(ldev, *buffer, *memory, 0);

	return VK_SUCCESS;
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
void createcomputepipeline()
{
	VkDescriptorPoolSize poolSizes[2] = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1;
	int ret = vkCreateDescriptorPool(logicaldevice, &poolInfo, 0, &descriptorPool);
	if(VK_SUCCESS != ret){
		printf("error@vkCreateDescriptorPool\n");
		return;
	}
	printf("DescriptorPool=%p\n", descriptorPool);


	VkDescriptorSetLayoutBinding setLayoutBindings = {};
	setLayoutBindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setLayoutBindings.descriptorCount = 1;
	setLayoutBindings.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo descsetlayoutinfo = {};
	descsetlayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descsetlayoutinfo.bindingCount = 1;
	descsetlayoutinfo.pBindings = &setLayoutBindings;
	ret = vkCreateDescriptorSetLayout(logicaldevice, &descsetlayoutinfo, 0, &descriptorSetLayout);
	if(VK_SUCCESS != ret){
		printf("error@vkCreateDescriptorSetLayout\n");
		return;
	}
	printf("DescriptorSetLayout=%p\n", descriptorSetLayout);


	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	ret = vkCreatePipelineLayout(logicaldevice, &pipelineLayoutInfo, 0, &pipelineLayout);
	if(VK_SUCCESS != ret){
		printf("error@vkCreatePipelineLayout\n");
		return;
	}
	printf("PipelineLayout=%p\n", pipelineLayout);


	VkDescriptorSetAllocateInfo descsetallocInfo = {};
	descsetallocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descsetallocInfo.descriptorPool = descriptorPool;
	descsetallocInfo.descriptorSetCount = 1;
	descsetallocInfo.pSetLayouts = &descriptorSetLayout;
	ret = vkAllocateDescriptorSets(logicaldevice, &descsetallocInfo, &descriptorSet);
	if(VK_SUCCESS != ret){
		printf("error@vkAllocateDescriptorSets\n");
		return;
	}
	printf("DescriptorSets=%p\n", descriptorSet);


	//shader
	ret = createShaderModule(&shaderModule, "shader.comp.spv");
	if(ret < 0){
		printf("error:%d@createShaderModule:compute\n",ret);
		return;
	}
	printf("ShaderModule=%p\n", shaderModule);

	VkPipelineShaderStageCreateInfo shaderStageinfo = {};
	shaderStageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageinfo.module = shaderModule;
	shaderStageinfo.pName = "main";


	//pipeline
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	ret = vkCreatePipelineCache(logicaldevice, &pipelineCacheCreateInfo, 0, &pipelineCache);
	if(VK_SUCCESS != ret){
		printf("error@vkCreatePipelineCache\n");
		return;
	}
	printf("PipelineCache=%p\n", pipelineCache);

	VkComputePipelineCreateInfo computePipelineCreateInfo = {};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.stage = shaderStageinfo;
	computePipelineCreateInfo.layout = pipelineLayout;
	ret = vkCreateComputePipelines(logicaldevice, pipelineCache, 1, &computePipelineCreateInfo, 0, &computepipeline);
	if(VK_SUCCESS != ret){
		printf("error@vkCreateComputePipelines\n");
		return;
	}
	printf("ComputePipelines=%p\n", computepipeline);


	//command buffer
	VkCommandBufferAllocateInfo cmdbufallocInfo = {};
	cmdbufallocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdbufallocInfo.commandPool = computePool;
	cmdbufallocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdbufallocInfo.commandBufferCount = 1;
	ret = vkAllocateCommandBuffers(logicaldevice, &cmdbufallocInfo, &commandBuffer);
	if(VK_SUCCESS != ret){
		printf("error@vkAllocateCommandBuffers\n");
		return;
	}
	printf("CommandBuffers=%p\n", commandBuffer);


	//fence
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	ret = vkCreateFence(logicaldevice, &fenceCreateInfo, 0, &computefence);
	if(VK_SUCCESS != ret){
		printf("error@vkCreateFence\n");
		return;
	}
	printf("Fence=%p\n", computefence);
}


void vulkan_myctx_create()
{
	physicaldevice = vulkan_device_create(2, 0);
	if(0 == physicaldevice)return;

	vulkan_physicaldevice_logicdevice(&physicaldevice, &logicaldevice);

	vulkan_computequeue_computepool(&computeQueue, &computePool);

	//compute pipeline
	createcomputepipeline();

	//cpu mem
	createBuffer(
		&hostBuffer,
		&hostMemory,
		physicaldevice,
		logicaldevice,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		bufferSize,
		0
	);
	printf("cpumem: fd=%p,mem=%p\n", hostBuffer, hostMemory);

	//gpu mem
	createBuffer(
		&deviceBuffer,
		&deviceMemory,
		physicaldevice,
		logicaldevice,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		0
	);
	printf("gpumem: fd=%p,mem=%p\n", deviceBuffer, deviceMemory);
}
void vulkan_myctx_delete()
{
	vkQueueWaitIdle(computeQueue);
/*
	vkDestroyBuffer(logicaldevice, deviceBuffer, 0);
	vkFreeMemory(logicaldevice, deviceMemory, 0);
	vkDestroyBuffer(logicaldevice, hostBuffer, 0);
	vkFreeMemory(logicaldevice, hostMemory, 0);

	vkDestroyPipelineLayout(logicaldevice, pipelineLayout, 0);
	vkDestroyDescriptorSetLayout(logicaldevice, descriptorSetLayout, 0);
	vkDestroyDescriptorPool(logicaldevice, descriptorPool, 0);
	vkDestroyPipeline(logicaldevice, pipeline, 0);
	vkDestroyPipelineCache(logicaldevice, pipelineCache, 0);
	vkDestroyFence(logicaldevice, fence, 0);
	vkDestroyCommandPool(logicaldevice, commandPool, 0);
	vkDestroyShaderModule(logicaldevice, shaderModule, 0);
*/
	vulkan_device_delete(physicaldevice);
}




void drawframe()
{
	int ret;

	//update data
	VkDescriptorBufferInfo bufferDescriptor = {};
	bufferDescriptor.buffer = deviceBuffer;
	bufferDescriptor.offset = 0;
	bufferDescriptor.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptorWrites[2] = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferDescriptor;
	vkUpdateDescriptorSets(logicaldevice, 1, descriptorWrites, 0, NULL);


	//copy from cpu to gpu
	unsigned int* tmp;
	vkMapMemory(logicaldevice, hostMemory, 0, VK_WHOLE_SIZE, 0, (void*)&tmp);

	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = hostMemory;
	mappedRange.offset = 0;
	mappedRange.size = VK_WHOLE_SIZE;
	vkInvalidateMappedMemoryRanges(logicaldevice, 1, &mappedRange);

	int x,y,z;
	for(z=0;z<256;z++){
	for(y=0;y<1024;y++){
	for(x=0;x<1024;x++)tmp[z*1024*1024 + y*1024 + x] = 2*1000*1000*1000;
	}
	}

	vkUnmapMemory(logicaldevice, hostMemory);


	//compute work
	VkCommandBufferBeginInfo cmdBufbeginInfo = {};
	cmdBufbeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	ret = vkBeginCommandBuffer(commandBuffer, &cmdBufbeginInfo);

	//command copy from cpu to gpu
	VkBufferCopy copyRegion = {};
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, hostBuffer, deviceBuffer, 1, &copyRegion);

	// Barrier to ensure that input buffer transfer is finished before compute shader reads from it
	VkBufferMemoryBarrier bufferBarrier = {};
	bufferBarrier.buffer = deviceBuffer;
	bufferBarrier.size = VK_WHOLE_SIZE;
	bufferBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, 0,
		1, &bufferBarrier,
		0, 0);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computepipeline);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

	vkCmdDispatch(commandBuffer, 1024/8, 1024/8, 256/8);

	// Barrier to ensure that shader writes are finished before buffer is read back from GPU
	bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	bufferBarrier.buffer = deviceBuffer;
	bufferBarrier.size = VK_WHOLE_SIZE;
	bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, 0,
		1, &bufferBarrier,
		0, 0);

	// Read back to host visible buffer
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, deviceBuffer, hostBuffer, 1, &copyRegion);

	// Barrier to ensure that buffer copy is finished before host reading from it
	bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	bufferBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
	bufferBarrier.buffer = hostBuffer;
	bufferBarrier.size = VK_WHOLE_SIZE;
	bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		0,
		0, 0,
		1, &bufferBarrier,
		0, 0);

	ret = vkEndCommandBuffer(commandBuffer);

	// Submit compute work
	vkResetFences(logicaldevice, 1, &computefence);
	const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo computeSubmitInfo = {};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(computeQueue, 1, &computeSubmitInfo, computefence);

	vkWaitForFences(logicaldevice, 1, &computefence, VK_TRUE, UINT64_MAX);


	//copy from gpu to cpu
	vkMapMemory(logicaldevice, hostMemory, 0, VK_WHOLE_SIZE, 0, (void*)&tmp);

	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = hostMemory;
	mappedRange.offset = 0;
	mappedRange.size = VK_WHOLE_SIZE;
	vkInvalidateMappedMemoryRanges(logicaldevice, 1, &mappedRange);

	printf("z=0,y=0\n");
	for(y=0;y<16;y++){
		for(x=0;x<8;x++)printf("%u ", tmp[y*1024+x]);
		printf("... ... %u\n", tmp[y*1024+1023]);
	}
	printf("z=0,y=1008\n");
	for(y=1024-16;y<1024;y++){
		for(x=0;x<8;x++)printf("%u ", tmp[y*1024+x]);
		printf("... ... %u\n", tmp[y*1024+1023]);
	}
	printf("z=7,y=1008\n");
	for(y=1024-16;y<1024;y++){
		for(x=0;x<8;x++)printf("%u ", tmp[7*1024*1024 + y*1024 + x]);
		printf("... ... %u\n", tmp[7*1024*1024 + y*1024 + 1023]);
	}
	printf("z=255,y=1008\n");
	for(y=1024-16;y<1024;y++){
		for(x=0;x<8;x++)printf("%u ", tmp[255*1024*1024 + y*1024 + x]);
		printf("... ... %u\n", tmp[255*1024*1024 + y*1024 + 1023]);
	}

	vkUnmapMemory(logicaldevice, hostMemory);
}
