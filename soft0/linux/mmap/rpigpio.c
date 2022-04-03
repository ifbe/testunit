#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


char* val2str[8] = {
	"input","output","alt5","alt4",
	"alt0", "alt1",  "alt2","alt3"
};


int main()
{
	int _dev_mem=open("/dev/gpiomem",O_RDWR|O_SYNC);
	printf("open:%x\n",_dev_mem);

	unsigned int* virtual=mmap(
                NULL,
                0x1000,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                _dev_mem,
                0
	);
	printf("mmap:%llx\n",virtual);

	int x,y;
	unsigned int val;
	for(y=0;y<5;y++){
		val = virtual[y];
		printf("GPFSEL%d=%x\n", y, val);
		for(x=0;x<10;x++){
			printf("%d: func=%d(%s)\n", y*10+x, val&7, val2str[val&7]);
			val >>= 3;
		}
	}
	return 0;
}
