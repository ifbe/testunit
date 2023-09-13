#include <stdio.h>
#include <cuda_runtime.h>

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
#include <time.h>
u64 time_in_ns()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (u64)t.tv_sec*1000*1000*1000 + t.tv_nsec;
}
#endif

__global__ void muladd_kernel(float* out, float* vec, float* mat, int xdim, int ydim)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int x;
	float f = 0.0;
	for(x=0;x<xdim;x++)f += mat[idx*xdim + x] * vec[x];
	out[idx] = f;
}

void printoutput(float* data, int len)
{
	int y;
	printf("[%d,%d)\n", 0, 31);
	for(y=0;y<2;y++){
		printf("%.1f, %.1f, %.1f, %.1f......%.1f, %.1f\n", data[y*16+0], data[y*16+1], data[y*16+2], data[y*16+3], data[y*16+14], data[y*16+15]);
	}
	printf("[%d,%d)\n", 16*254, 16*256-1);
	for(y=254;y<256;y++){
		printf("%.1f, %.1f, %.1f, %.1f......%.1f, %.1f\n", data[y*16+0], data[y*16+1], data[y*16+2], data[y*16+3], data[y*16+14], data[y*16+15]);
	}
}




static int xdim = 16384;
static int ydim = 32000;
static int vecbyte = xdim * sizeof(float);
static int matbyte = xdim * ydim * sizeof(float);
static float *cpuout = 0;
static float *cpuvec = 0;
static float *cpumat = 0;
static float *gpuout = 0;
static float *gpuvec = 0;
static float *gpumat = 0;
void compute()
{
	xdim = 4096;
	ydim = 4096;
	vecbyte = xdim * sizeof(float);
	matbyte = xdim * ydim * sizeof(float);
	dim3 threads = dim3(512, 1, 1);
	dim3 blocks  = dim3(ydim/512, 1, 1);

	int x,y;
	for(y=0;y<ydim;y++){
		for(x=0;x<xdim;x++){
				cpumat[y*xdim + x] = (x==y) ? 1.0 : 0.0;
		}
		cpumat[y*xdim+17] += 100*1000.0;
	}
	for(x=0;x<xdim;x++){
		cpuvec[x] = x*1.0;
	}

	// create cuda event handles
	u64 time[4];
	cudaEvent_t event[4];
	for(int i=0;i<4;i++)cudaEventCreate(&event[i]);

	cudaDeviceSynchronize();

	// asynchronously issue work to the GPU (all to stream 0)
	time[0] = time_in_ns();
	cudaEventRecord(event[0], 0);
	cudaMemcpyAsync(gpuvec, cpuvec, vecbyte, cudaMemcpyHostToDevice, 0);
	cudaMemcpyAsync(gpumat, cpumat, matbyte, cudaMemcpyHostToDevice, 0);
	cudaEventRecord(event[1], 0);
	muladd_kernel<<<blocks, threads, 0, 0>>>(gpuout, gpuvec, gpumat, xdim, ydim);
	cudaEventRecord(event[2], 0);
	cudaMemcpyAsync(cpuout, gpuout, vecbyte, cudaMemcpyDeviceToHost, 0);
	cudaEventRecord(event[3], 0);
	time[1] = time_in_ns();

	// have CPU do some work while waiting for stage 1 to finish
	unsigned long int counter=0;
	while (cudaEventQuery(event[3]) == cudaErrorNotReady)
	{
		counter++;
	}
	time[2] = time_in_ns();

	float gputime[3] = {};
	for(int i=0;i<3;i++)cudaEventElapsedTime(&gputime[i], event[i], event[i+1]);
	printf("time spent executing by the GPU: %f, %f, %f\n", gputime[0], gputime[1], gputime[2]);

	printf("time spent executing by the CPU: %f, %f\n", (time[1]-time[0])*1e-6, (time[2]-time[1])*1e-6);

	printf("cycle spent executing by the CPU: %lu\n", counter);

	printoutput(cpuout, ydim);

	for(int i=0;i<4;i++)cudaEventDestroy(event[i]);
}




void backend_init()
{
	u64 t0 = time_in_ns();
	cudaSetDevice(0);

	// allocate host memory
	cudaMallocHost((void **)&cpuout, vecbyte);
	cudaMallocHost((void **)&cpuvec, vecbyte);
	cudaMallocHost((void **)&cpumat, matbyte);

	// allocate device memory
	cudaMalloc((void **)&gpuout, vecbyte);
	cudaMalloc((void **)&gpuvec, vecbyte);
	cudaMalloc((void **)&gpumat, matbyte);
	//cudaMemset(gpumem, 255, nbytes);

	u64 t1 = time_in_ns();
	printf("backend_init costtime: %f\n", (t1-t0)*1e-6);
}
void backend_exit()
{
	u64 t0 = time_in_ns();

	cudaFree(gpumat);
	cudaFree(gpuvec);
	cudaFree(gpuout);
	cudaFreeHost(cpumat);
	cudaFreeHost(cpuvec);
	cudaFreeHost(cpuout);

	u64 t1 = time_in_ns();
	printf("backend_exit costtime: %f\n", (t1-t0)*1e-6);
}
int main(int argc, char *argv[])
{
	backend_init();
	compute();
	backend_exit();
}
