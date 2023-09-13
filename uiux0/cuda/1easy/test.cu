#include <stdio.h>
#include <cuda_runtime.h>

__global__ void increment_kernel(float* g_data)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	g_data[idx] = idx*1.0;
}

void printoutput(float* data, int len)
{
	if(len>48)len=48;
	for (int i = 0; i < len; i++){
		printf("%4.1f%c", data[i], ((i&0xf)==15)?'\n':' ');
	}
}
int main(int argc, char *argv[])
{
	cudaSetDevice(0);

	int n = 16 * 1024 * 1024;
	int nbytes = n * sizeof(float);

	dim3 threads = dim3(512, 1, 1);
	dim3 blocks  = dim3(n / threads.x, 1, 1);

	// allocate host memory
	float *cpumem = 0;
	cudaMallocHost((void **)&cpumem, nbytes);
	memset(cpumem, 0, nbytes);

	// allocate device memory
	float *gpumem = 0;
	cudaMalloc((void **)&gpumem, nbytes);
	cudaMemset(gpumem, 255, nbytes);

	// create cuda event handles
	cudaEvent_t event[4];
	for(int i=0;i<4;i++)cudaEventCreate(&event[i]);

	cudaDeviceSynchronize();

	// asynchronously issue work to the GPU (all to stream 0)
	cudaEventRecord(event[0], 0);
	cudaMemcpyAsync(gpumem, cpumem, nbytes, cudaMemcpyHostToDevice, 0);
	cudaEventRecord(event[1], 0);
	increment_kernel<<<blocks, threads, 0, 0>>>(gpumem);
	cudaEventRecord(event[2], 0);
	cudaMemcpyAsync(cpumem, gpumem, nbytes, cudaMemcpyDeviceToHost, 0);
	cudaEventRecord(event[3], 0);

	// have CPU do some work while waiting for stage 1 to finish
	unsigned long int counter=0;
	while (cudaEventQuery(event[3]) == cudaErrorNotReady)
	{
		counter++;
	}

	float gputime[3] = {};
	for(int i=0;i<3;i++)cudaEventElapsedTime(&gputime[i], event[i], event[i+1]);
	printf("time spent executing by the GPU: %.f, %f, %f\n", gputime[0], gputime[1], gputime[2]);

	printf("cycle spent executing by the CPU: %lu\n", counter);

	printoutput(cpumem, n);

	// release resources
	for(int i=0;i<4;i++)cudaEventDestroy(event[i]);
	cudaFreeHost(cpumem);
	cudaFree(gpumem);
}
