#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef O_BINARY
        #define O_BINARY 0x0
#endif


struct namedata{
char name[16];
__int128 data;
};
typedef struct namedata _namedata_;


struct headtail{
_namedata_ delta_root;
_namedata_ delta_peer;
_namedata_ pool_uuid;
_namedata_ pool_myid;
char padd[0x1000 - 16*4];
};
static struct headtail head;
static struct headtail tail;


struct mainroot{
_namedata_ delta_head;
_namedata_ delta_tail;
_namedata_ vpos_self;
_namedata_ vpos_pttree;
_namedata_ vpos_dirtree;
char padd[0x1000-48];
};
static struct mainroot root;


void print128(unsigned char* buf)
{
	int j;
	for(j=15;j>=0;j--)printf("%02x",buf[j]);
	printf("\n");
}
int myfs_create(int argc, char** argv)
{
	if(argc < 2){
		printf("explain: progname devicenode firstbyte_usable firstbyte_after\n");
		printf("example: ./myfs.create /dev/loop0p1 0x0 0x3000\n");
		return 0;
	}

	//get arg first last
	int first = -1,last = -1, half;
	if(argc >= 3){
		first = strtol(argv[2], 0, 16);

		if(argc >= 4){
			last = strtol(argv[3], 0, 16);
		}
	}
	printf("arg.first=%x\n", first);
	printf("arg.last=%x\n", last);
	if(first&0xfff){
		printf("first not 4k aligned\n");
		return 0;
	}
	if(last&0xfff){
		printf("last not 4k aligned\n");
		return 0;
	}
	if(first+0x3000 > last){
		printf("minsize=last-first must greater than 0x3000\n");
		return 0;
	}
	printf("----\n");


	//get device first last
	int fd = open("test.img", O_CREAT|O_RDWR|O_TRUNC|O_BINARY, S_IRWXU|S_IRWXG|S_IRWXO);
	int diskfirst = 0;
	int disklast = 0x10000;
	printf("disk.first=%x\n", diskfirst);
	printf("disk.last=%x\n", disklast);
	printf("----\n");


	//compute decision
	first = first&0xfffff000;
	last = (last&0xfffff000)-0x1000;
	half = ((first+last)/2) & 0xfffff000;
	printf("final: first=%x\n", first);
	printf("final: half=%x\n", half);
	printf("final: last=%x\n", last);
	printf("----\n");


	//head
	strncpy(head.delta_root.name, "byteoffsroot -> ", 16);
	head.delta_root.data = half-first;
	strncpy(head.delta_peer.name, "byteoffstail -> ", 16);
	head.delta_peer.data = last-first;


	//tail
	strncpy(tail.delta_root.name, "byteoffsroot -> ", 16);
	tail.delta_root.data = half-last;
	strncpy(tail.delta_peer.name, "byteoffshead -> ", 16);
	tail.delta_peer.data =first-last;


	//root
	strncpy(root.delta_head.name, "byteoffshead -> ", 16);
	root.delta_head.data =first-half;
	strncpy(root.delta_tail.name, "byteoffstail -> ", 16);
	root.delta_tail.data = last-half;

	strncpy(root.vpos_self.name, "bytevposself -> ", 16);
	root.vpos_self.data = 0x80000000;
	strncpy(root.vpos_pttree.name, "bytevpos__pt -> ", 16);
	root.vpos_pttree.data = 0xffffffffffffffff;
	strncpy(root.vpos_dirtree.name, "bytevpos_dir -> ", 16);
	root.vpos_dirtree.data = 0xffffffffffffffff;


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
int main(int argc, char** argv)
{
	return myfs_create(argc, argv);
}
