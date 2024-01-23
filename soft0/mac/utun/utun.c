#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/kern_event.h>
#include <sys/kern_control.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if_utun.h>

int32_t open_utun(uint64_t num) {
	int err;

	int fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
	if (fd < 0) {
		printf("socket PF_SYSTEM SYSPROTO_CONTROL: errno=%d\n", errno);
		return fd;
	}

	struct ctl_info info;
	memset(&info, 0, sizeof (info));
	strncpy(info.ctl_name, UTUN_CONTROL_NAME, strlen(UTUN_CONTROL_NAME));
	err = ioctl(fd, CTLIOCGINFO, &info);
	if (err < 0) {
		printf("ioctl CTLIOCGINFO: errno=%d\n", errno);
		close(fd);
		return err;
	}

	struct sockaddr_ctl addr = {};
	addr.sc_len = sizeof(addr);
	addr.sc_family = AF_SYSTEM;
	addr.ss_sysaddr = AF_SYS_CONTROL;
	addr.sc_id = info.ctl_id;
	addr.sc_unit = num + 1; // utunX where X is sc.sc_unit -1

	err = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (err < 0) {
		printf("connect: errno=%d\n", errno);
		close(fd);
		return err;
	}

	return fd;
}

void printmemory(unsigned char* buf, int len)
{
	int j;
	for(j=0;j<len;j++){
		printf("%02x%c", buf[j], ((j%16)==15) ? '\n' : ' ');
	}
	printf("\n");
}

int main(int argc, char** argv){
	if(argc < 2){
		printf("./a.out 4\n");
		return -1;
	}

	int id = atoi(argv[1]);

	int fd = open_utun(id);
	if(fd<0)return -1;

	printf("set ip:\n");
	printf("sudo ifconfig utun%d selfaddr peeraddr\n", id);
	printf("set route:\n");
	printf("sudo route add -net 192.168.88.33/24 -interface utun%d", id);
	printf("del route:\n");
	printf("sudo route delete -net 192.168.88.0/24 -interface utun%d", id);

	int ret;
	unsigned char buf[65536];
	while(1){
		ret = read(fd, buf, 65536);

		printf("ret=%d\n", ret);
		printmemory(buf, ret);

		usleep(10*1000);
	}
	return 0;
}
