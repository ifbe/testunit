#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef O_BINARY
        #define O_BINARY 0x0
#endif


struct headtail{
__int128 delta_root;
__int128 delta_peer;
__int128 pool_uuid;
__int128 pool_myid;
char padd[0x1000 - 16*4];
};
static struct headtail head;
static struct headtail tail;


struct mainroot{
__int128 vpos_self;
__int128 vpos_pttree;
__int128 vpos_dirtree;
char padd[0x1000-48];
};
static struct mainroot root;


void print128(unsigned char* buf)
{
	int j;
	for(j=15;j>=0;j--)printf("%02x",buf[j]);
	printf("\n");
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out /dev/loop0p1 x y\n");
		return 0;
	}

	//get arg first last
	int first = -1,last = -1, half;
	if(argc >= 3){
		first = atoi(argv[2]);

		if(argc >= 4){
			last = atoi(argv[3]);
		}
	}
	printf("arg:first=%x\n", first);
	printf("arg:last=%x\n", last);
	printf("----\n");

	//get device first last
	int fd = open("test.img", O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IRWXU|S_IRWXG|S_IRWXO);
	first = 0&0xfffff000;
	last = 0xf000&0xfffff000;
	half = ( (first>>13) + (last>>13) ) << 12;
	printf("dev:first=%x\n", first);
	printf("dev:last=%x\n", last);
	printf("----\n");

	//point head and tail to realroot
	head.delta_root = half-first;
	head.delta_peer = last-first;

	tail.delta_root = half-last;
	tail.delta_peer = first-last;

	//filesystem information
	root.vpos_self = 0x80000000;

	//write three 4KB metadata
	printf("writing...\n");
	lseek(fd, first, SEEK_SET);
	write(fd, &head, 0x1000);
	lseek(fd, last, SEEK_SET);
	write(fd, &tail, 0x1000);
	lseek(fd, half, SEEK_SET);
	write(fd, &root, 0x1000);

	printf("written\n");
	close(fd);

	return 0;
}
