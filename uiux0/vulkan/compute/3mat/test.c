#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>


#define u64 unsigned long long
#ifdef _WIN32
#include <windows.h>
u64 time_in_ns()
{
	LARGE_INTEGER count,freq;
	int ret = QueryPerformanceFrequency(&freq);
	if(ret && freq.QuadPart){
		ret = QueryPerformanceCounter(&count);
		//say("count=%lld,freq=%lld,time=%lld\n", count.QuadPart, freq.QuadPart, (u64)count.QuadPart*1000*1000 / (freq.QuadPart/1000));
		if(ret && count.QuadPart)return (u64)count.QuadPart*1000*1000 / (freq.QuadPart/1000);		//without (u64)=overflow, 10^9*count/freq = overflow
	}

	return 1000 * 1000 * GetTickCount64();
}
#elif __APPLE__
#include <mach/mach_time.h>
#define lseek64 lseek
u64 time_in_ns()
{
	return mach_absolute_time();
}
#else
u64 time_in_ns()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (u64)t.tv_sec*1000*1000*1000 + t.tv_nsec;
}
#endif


void* vulkan_device_create(int, void*);
void vulkan_device_delete(void*);
void vulkan_physicaldevice_logicdevice(VkPhysicalDevice* pdev, VkDevice* ldev);
void vulkan_computequeue_computepool(VkQueue* queue, VkCommandPool* pool);
//
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
VkPipeline computepipeline;
VkPipelineCache pipelineCache;
VkPipelineLayout pipelineLayout;
//
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet;
VkDescriptorSetLayout descriptorSetLayout;
//
VkBuffer hostBuffer[3];
VkDeviceMemory hostMemory[3];
VkBuffer deviceBuffer[3];
VkDeviceMemory deviceMemory[3];
//
#define xdim 4096
#define ydim 4096
int vectorbuffersize = 4*xdim;
int matrixbuffersize = 4*xdim*ydim;
unsigned int pushconst[4] = {xdim, ydim, 0, 0};





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
	//descpool
	VkDescriptorPoolSize poolSizes[2] = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[0].descriptorCount = 3;

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


	//descsetlayoutbinding
	VkDescriptorSetLayoutBinding setLayoutBindings[3] = {};
	setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setLayoutBindings[0].descriptorCount = 1;
	setLayoutBindings[0].binding = 0;
	setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setLayoutBindings[1].descriptorCount = 1;
	setLayoutBindings[1].binding = 1;
	setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setLayoutBindings[2].descriptorCount = 1;
	setLayoutBindings[2].binding = 2;
	setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


	//descriptorSetLayout = descsetlayoutbinding + xxx
	VkDescriptorSetLayoutCreateInfo descsetlayoutinfo = {};
	descsetlayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descsetlayoutinfo.bindingCount = 3;
	descsetlayoutinfo.pBindings = setLayoutBindings;
	ret = vkCreateDescriptorSetLayout(logicaldevice, &descsetlayoutinfo, 0, &descriptorSetLayout);
	if(VK_SUCCESS != ret){
		printf("error@vkCreateDescriptorSetLayout:%d\n",ret);
		return;
	}
	printf("DescriptorSetLayout=%p\n", descriptorSetLayout);


	//descriptorSet = descriptorsetlayout + xxx
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


	VkPushConstantRange pushconstant;
	pushconstant.offset = 0;
	pushconstant.size = 16;
	pushconstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


	//pipelinelayoutpipelineLayout = descriptorsetlayout + xxx
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushconstant;
	ret = vkCreatePipelineLayout(logicaldevice, &pipelineLayoutInfo, 0, &pipelineLayout);
	if(VK_SUCCESS != ret){
		printf("error@vkCreatePipelineLayout\n");
		return;
	}
	printf("PipelineLayout=%p\n", pipelineLayout);


	//pipelinecache
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	ret = vkCreatePipelineCache(logicaldevice, &pipelineCacheCreateInfo, 0, &pipelineCache);
	if(VK_SUCCESS != ret){
		printf("error@vkCreatePipelineCache\n");
		return;
	}
	printf("PipelineCache=%p\n", pipelineCache);


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


	//computepipeline = shader + pipelinelayout + pipeline cache
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

	for(int j=0;j<3;j++){
		//gpu mem
		printf("gpumem%d: allocing\n", j);
		createBuffer(
			&deviceBuffer[j],
			&deviceMemory[j],
			physicaldevice,
			logicaldevice,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			(j!=2) ? vectorbuffersize : matrixbuffersize,
			0
		);
		printf("gpumem%d: fd=%p,mem=%p\n", j, deviceBuffer[j], deviceMemory[j]);

		//cpu mem
		printf("cpumem%d: allocing\n", j);
		createBuffer(
			&hostBuffer[j],
			&hostMemory[j],
			physicaldevice,
			logicaldevice,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			(j!=2) ? vectorbuffersize : matrixbuffersize,
			0
		);
		printf("cpumem%d: fd=%p,mem=%p\n", j, hostBuffer[j], hostMemory[j]);
	}
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




/*
time spent on gpu(including copy_cpu_to_gpu and copy_gpu_to_cpu):
hd530: 28s
hd770: 12s
gtx1060 0.7s
*/
void drawframe()
{
	printf("prepare start\n");

	//update data
	VkDescriptorBufferInfo bufferDescriptor[3] = {};
	VkWriteDescriptorSet descriptorWrites[3] = {};
	for(int j=0;j<3;j++){
		bufferDescriptor[j].buffer = deviceBuffer[j];
		bufferDescriptor[j].offset = 0;
		bufferDescriptor[j].range = VK_WHOLE_SIZE;

		descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[j].dstSet = descriptorSet;
		descriptorWrites[j].dstBinding = j;
		descriptorWrites[j].dstArrayElement = 0;
		descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[j].descriptorCount = 1;
		descriptorWrites[j].pBufferInfo = &bufferDescriptor[j];
	}
	vkUpdateDescriptorSets(logicaldevice, 3, descriptorWrites, 0, NULL);


	//cpumem write
	float* tmp0;
	float* tmp1;
	float* tmp2;
	vkMapMemory(logicaldevice, hostMemory[0], 0, VK_WHOLE_SIZE, 0, (void*)&tmp0);
	vkMapMemory(logicaldevice, hostMemory[1], 0, VK_WHOLE_SIZE, 0, (void*)&tmp1);
	vkMapMemory(logicaldevice, hostMemory[2], 0, VK_WHOLE_SIZE, 0, (void*)&tmp2);

	VkMappedMemoryRange mappedRange[2] = {};
	mappedRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange[0].memory = hostMemory[1];
	mappedRange[0].offset = 0;
	mappedRange[0].size = VK_WHOLE_SIZE;
	mappedRange[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange[1].memory = hostMemory[2];
	mappedRange[1].offset = 0;
	mappedRange[1].size = VK_WHOLE_SIZE;
	vkInvalidateMappedMemoryRanges(logicaldevice, 2, mappedRange);

	int x,y;
	for(y=0;y<ydim;y++){
		for(x=0;x<xdim;x++){
			tmp2[y*xdim + x] = (x==y) ? 1.0 : 0.0;
		}
		tmp2[y*xdim+17] += 100*1000.0;
	}
	for(x=0;x<xdim;x++){
		tmp1[x] = x*1.0;
	}

	vkUnmapMemory(logicaldevice, hostMemory[2]);
	vkUnmapMemory(logicaldevice, hostMemory[1]);
	vkUnmapMemory(logicaldevice, hostMemory[0]);
	printf("prepare finish\n\n");


	printf("gpu compute\n");
	unsigned long long t0 = time_in_ns();

	//compute work
	VkCommandBufferBeginInfo cmdBufbeginInfo = {};
	cmdBufbeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	int ret = vkBeginCommandBuffer(commandBuffer, &cmdBufbeginInfo);

	//command copy from cpu to gpu
	VkBufferCopy matrixcopyregion = {};
	matrixcopyregion.size = matrixbuffersize;
	vkCmdCopyBuffer(commandBuffer, hostBuffer[2], deviceBuffer[2], 1, &matrixcopyregion);

	VkBufferCopy vectorcopyregion = {};
	vectorcopyregion.size = vectorbuffersize;
	vkCmdCopyBuffer(commandBuffer, hostBuffer[1], deviceBuffer[1], 1, &vectorcopyregion);

	// Barrier to ensure that input buffer transfer is finished before compute shader reads from it
	VkBufferMemoryBarrier bufferBarrier[2] = {};
	bufferBarrier[0].buffer = deviceBuffer[1];
	bufferBarrier[0].size = VK_WHOLE_SIZE;
	bufferBarrier[0].srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	bufferBarrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	bufferBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier[1].buffer = deviceBuffer[2];
	bufferBarrier[1].size = VK_WHOLE_SIZE;
	bufferBarrier[1].srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	bufferBarrier[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	bufferBarrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, 0,
		2, &bufferBarrier[0],
		0, 0);

	//compute command
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computepipeline);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, 16, pushconst);

	vkCmdDispatch(commandBuffer, 64/32, 64/32, 1/1);

	// Barrier to ensure that shader writes are finished before buffer is read back from GPU
	bufferBarrier[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	bufferBarrier[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	bufferBarrier[0].buffer = deviceBuffer[0];
	bufferBarrier[0].size = VK_WHOLE_SIZE;
	bufferBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, 0,
		1, &bufferBarrier[0],
		0, 0);

	// Read back to host visible buffer
	vectorcopyregion.size = vectorbuffersize;
	vkCmdCopyBuffer(commandBuffer, deviceBuffer[0], hostBuffer[0], 1, &vectorcopyregion);

	// Barrier to ensure that buffer copy is finished before host reading from it
	bufferBarrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	bufferBarrier[0].dstAccessMask = VK_ACCESS_HOST_READ_BIT;
	bufferBarrier[0].buffer = hostBuffer[0];
	bufferBarrier[0].size = VK_WHOLE_SIZE;
	bufferBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		0,
		0, 0,
		1, &bufferBarrier[0],
		0, 0);

	ret = vkEndCommandBuffer(commandBuffer);

	unsigned long long t1 = time_in_ns();

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

	unsigned long long t2 = time_in_ns();

	//cpumem read
	vkMapMemory(logicaldevice, hostMemory[0], 0, VK_WHOLE_SIZE, 0, (void*)&tmp0);
	vkMapMemory(logicaldevice, hostMemory[1], 0, VK_WHOLE_SIZE, 0, (void*)&tmp1);
	vkMapMemory(logicaldevice, hostMemory[2], 0, VK_WHOLE_SIZE, 0, (void*)&tmp2);

	mappedRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange[0].memory = hostMemory[0];
	mappedRange[0].offset = 0;
	mappedRange[0].size = VK_WHOLE_SIZE;
	vkInvalidateMappedMemoryRanges(logicaldevice, 1, &mappedRange[0]);

	unsigned long long t3 = time_in_ns();
	printf("gpu finish\n\n");


	//evaluate gpu
	printf("gpu cost=%f+%f+%f\n", (t1-t0)*1e-9, (t2-t1)*1e-9, (t3-t2)*1e-9);

	printf("[0,32)\n");
	for(y=0;y<4;y++){
		for(x=0;x<4;x++)printf("%.3f ", tmp0[y*16+x]);
		printf("... ... %.3f\n", tmp0[y*16+15]);
	}
	printf("[4032,4096)\n");
	for(y=252;y<256;y++){
		for(x=0;x<4;x++)printf("%.3f ", tmp0[y*16+x]);
		printf("... ... %.3f\n", tmp0[y*16+15]);
	}
	printf("\n");


	//compute cpu
	printf("cpu compute\n");
	u64 ta = time_in_ns();
	for(y=0;y<ydim;y++){
		float tmp = 0.0;
		for(x=0;x<xdim;x++){
		tmp += tmp2[y*xdim+x] * tmp1[x];
		}
		tmp0[y] = tmp;
	}
	u64 tb = time_in_ns();
	printf("cpu finish\n\n");


	//evaluate cpu
	printf("cpu cost=%f\n",(tb-ta)*1e-9);

	printf("[0,32)\n");
	for(y=0;y<4;y++){
		for(x=0;x<4;x++)printf("%.3f ", tmp0[y*16+x]);
		printf("... ... %.3f\n", tmp0[y*16+15]);
	}
	printf("[4032,4096)\n");
	for(y=252;y<256;y++){
		for(x=0;x<4;x++)printf("%.3f ", tmp0[y*16+x]);
		printf("... ... %.3f\n", tmp0[y*16+15]);
	}
	printf("\n");


	vkUnmapMemory(logicaldevice, hostMemory[2]);
	vkUnmapMemory(logicaldevice, hostMemory[1]);
	vkUnmapMemory(logicaldevice, hostMemory[0]);
}
