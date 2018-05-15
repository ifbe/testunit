#include<stdio.h>
static void f0()
{}
static void f1()
{}
static void f2()
{}
static void f3()
{}
static void* func0 __attribute__((section(".data"))) = f0;
static void* func1 __attribute__((section(".data"))) = f1;
static void* func2 __attribute__((section(".data"))) = f2;
static void* func3 __attribute__((section(".data"))) = f3;

void main()
{
	printf("%llx,%llx,%llx,%llx\n",func0,func1,func2,func3);
}
