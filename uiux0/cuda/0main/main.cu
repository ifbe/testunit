#include<stdio.h>
#define DEV 0
/*  A sample program for nvcc compile */


//device code
__global__ void helloFromGPU()
{
    printf("Hello World from GPU...\n");
}

int main()
{
    printf("Hello World from CPU\n");    //host code

    cudaSetDevice(0);                   //device code
    helloFromGPU<<<1,10>>>();           //device code
    cudaDeviceReset();                  //device code
    return 0;
}
