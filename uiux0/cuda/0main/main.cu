#include<stdio.h>


__global__ void helloFromGPU()
{
    printf("Hello World from GPU...\n");
}

int main()
{
	int count = 0;
	int ret = cudaGetDeviceCount(&count);
	printf("cudaGetDeviceCount:count=%d,ret=%d\n", count, ret);
	if(count<=0)return 0;

	int j;
	char tmpstr[32];
	cudaDeviceProp prop;
	for(j=0;j<count;j++){
		ret = cudaGetDeviceProperties(&prop, j);
		printf("cudaGetDeviceProperties %d\n", j);
		printf("	name: [%s]\n", prop.name);
		printf("	totalGlobalMem: [%zx](%zdMB)\n", prop.totalGlobalMem, prop.totalGlobalMem/1024/1024);
		printf("	sharedMemPerBlock: [%zd]\n", prop.sharedMemPerBlock);
		printf("	regsPerBlock: [%d]\n", prop.regsPerBlock);
		printf("	warpSize: [%d]\n", prop.warpSize);
		printf("	memPitch: [%zd]\n", prop.memPitch);
		printf("	maxThreadsPerBlock: [%d]\n", prop.maxThreadsPerBlock);
		printf("	maxThreadsDim: [%d,%d,%d]\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
		printf("	maxGridSize: [%d,%d,%d]\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
		printf("	totalConstMem: [%zd]\n", prop.totalConstMem);
		printf("	version: [%d,%d]\n", prop.major, prop.minor);
		printf("	clockRate: [%d]\n", prop.clockRate);
		printf("	textureAlignment: [%zd]\n", prop.textureAlignment);
		printf("	deviceOverlap: [%d]\n", prop.deviceOverlap);
		printf("	multiProcessorCount: [%d]\n", prop.multiProcessorCount);
		printf("	kernelExecTimeoutEnabled: [%d]\n", prop.kernelExecTimeoutEnabled);
		printf("	integrated: [%d]\n", prop.integrated);
		printf("	canMapHostMemory: [%d]\n", prop.canMapHostMemory);
		printf("	computeMode: [%d]\n", prop.computeMode);
		printf("	maxTexture1D: [%d]\n", prop.maxTexture1D);
		printf("	maxTexture2D: [%d,%d]\n", prop.maxTexture2D[0], prop.maxTexture2D[1]);
		printf("	maxTexture3D: [%d,%d,%d]\n", prop.maxTexture3D[0], prop.maxTexture3D[1], prop.maxTexture3D[2]);
		printf("	concurrentKernels: [%d]\n", prop.concurrentKernels);

		cudaDeviceGetPCIBusId(tmpstr, 32, j);
		printf("	pcibus: [%s]\n", tmpstr);

		printf("\n");
	}

	cudaSetDevice(0);

	size_t avail,total;
	cudaMemGetInfo(&avail, &total);
	printf("cudaMemGetInfo: avail=%zdMB, total=%zdMB\n", avail/1024/1024, total/1024/1024);
	printf("\n");

	helloFromGPU<<<1,10>>>();

	cudaDeviceReset();
	return 0;
}
