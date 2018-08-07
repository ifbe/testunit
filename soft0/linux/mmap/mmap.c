#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
	int _dev_mem=open("/dev/mem",O_RDWR|O_SYNC);
	printf("open:%x\n",_dev_mem);

	unsigned char* virtual=mmap(
                NULL,
                0x100000,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                _dev_mem,
                0
	);
	printf("mmap:%llx\n",virtual);

	int x,y;
	for(y=0;y<0x10;y++)
	{
		for(x=0;x<0x10;x++)
		{
			printf("%.2x ",virtual[0xe0000 + (y*16) + x]);
		}
		printf("\n");
	}
	return 0;
}
