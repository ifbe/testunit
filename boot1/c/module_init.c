#include <stdio.h>
static int fuck(void)
{
        return 0;
}
static int shit(void)
{
        return 0;
}
int main()
{
        printf("%p,%p\n",fuck,shit);
        return 0;
}

typedef int (*initcall_t)(void);
static initcall_t f __attribute__((section(".initcall"))) = fuck;
static initcall_t s __attribute__((section(".exitcall"))) = shit;
