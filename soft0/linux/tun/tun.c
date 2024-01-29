#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


void print_icmp(unsigned char* buf, int len)
{
	int j,k;
	printf("icmp len=%d{\n", len);

	for(j=0;j<len;j++){
		k = !((j+1)&0xf);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	printf("}icmp\n");
}
void print_udp(unsigned char* buf, int len)
{
	printf("udp len=%d{\n", len);
	int j,k;
	for(j=0;j<4;j++){
		k = (j==3);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	printf("}udp\n");
}
void print_tcp(unsigned char* buf, int len)
{
	printf("tcp len=%d{\n", len);
	int j,k;
	for(j=0;j<4;j++){
		k = (j==3);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	printf("}tcp\n");
}
void print_unknown(unsigned char* buf, int len)
{
	printf("unknown len=%d{\n", len);
	int j,k;
	for(j=0;j<4;j++){
		k = (j==3);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	printf("}unknown\n");
}
void print_ipv4(unsigned char* buf, int len)
{
	int j,k;
	u8 proto = buf[9];
	printf("ipv4,proto=%x,len=%d{\n",proto,len);
	for(j=0;j<20;j++){
		k = (j==11)|(j==19);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}

	switch(proto){
	case 0x1:
		print_icmp(buf+20, len-20);
		break;
	case 0x6:
		print_tcp(buf+20, len-20);
		break;
	case 0x11:
		print_udp(buf+20, len-20);
		break;
	default:
		print_unknown(buf+20, len-20);
	}
	printf("}ipv4\n");
}
void print_ipv6(unsigned char* buf, int len)
{
	int j,k;
	u8 proto = buf[6];
	printf("ipv6 proto=%x,len=%d{\n",proto, len);

	for(j=0;j<40;j++){
		k = (j+1==len)|(j==7)|(j==7+16)|(j==7+32);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	print_unknown(buf+40, len-40);
	printf("}ipv6\n");
}
void print_arp(unsigned char* buf, int len)
{
	int j,k;
	printf("arp len=%d{\n", len);
	for(j=0;j<28;j++){
		k = (j==7)|(j==7+10)|(j==7+20);
		printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
	}
	printf("}arp\n");
}
void print_ethernet(unsigned char* buf, int len)
{
	int j,k;
	int type = (buf[12]<<8) | buf[13];
	printf("ethernet type=%x len=%d{\n", type, len);
	switch(type){
	case 0x0806:
		for(j=0;j<14;j++){
			k = (j==13);
			printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
		}
		print_arp(buf+14, len-14);
		break;
	case 0x8035:
		if(len > 0x20)len = 0x20;
		for(j=0;j<len;j++){
			k = (j+1==len)|(j==0x0f);
			printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
		}
		break;
	case 0x86dd:
		for(j=0;j<14;j++){
			k = (j==13);
			printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
		}
		print_ipv6(buf+14, len-14);
		break;
	case 0x0800:
		for(j=0;j<14;j++){
			k = (j==13);
			printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
		}
		print_ipv4(buf+14, len-14);
		break;
	default:
		if(len > 0x20)len = 0x20;
		for(j=0;j<len;j++){
			k = (j+1==len)|(j==0x0f);
			printf("%02x%c", buf[j], k*(0xa-0x20)+0x20);
		}
		break;
	}
	printf("}ethernet\n");
}
void print_packet(unsigned char* buf, int len)
{
	int j,k;
	printf("packet len=%d{\n", len);
	print_ethernet(buf, len);
	printf("}packet\n");
}




u16 swap16(u16 in)
{
	return (in>>8) + ((in&0xff)<<8);
}
void icmp_checksum(u8* buf, int len)
{
	u16* buf16 = (u16*)buf;
	int len16 = len/2;

	int j;
	u32 sum = swap16(buf16[0]);
	for(j=2;j<len16;j++)sum += swap16(buf16[j]);

	while(sum>>16)sum = (sum&0xffff) + (sum>>16);

	buf16[1] = swap16(0xffff - sum);
}
void reply_icmp(u8* buf, int len, u8* tmp, int max)
{
	printf("build icmp\n");
	int j,k;
	u8* icmp_req = buf+14+20;
	u8* icmp_echo = tmp;
	for(j=1;j<len;j++){
		icmp_echo[j] = icmp_req[j];
	}
	icmp_echo[0] = 0;

	for(j=0;j<len-14-20;j++){
		k = !((j+1)&0xf);
		printf("%02x%c", tmp[j], k*(0xa-0x20)+0x20);
	}
	icmp_checksum(tmp, len-14-20);
}
void reply_ip(u8* buf, int len, u8* tmp, int max)
{
	printf("build ip\n");
	int j,k;
	u8* ip_req = buf+14;
	u8* ip_echo = tmp;
	for(j=0;j<20;j++){
		ip_echo[j] = ip_req[j];
	}

	u32 src = *(unsigned int*)(ip_req+12);
	u32 dst = *(unsigned int*)(ip_req+12+4);
	*(unsigned int*)(ip_echo+12) = dst;
	*(unsigned int*)(ip_echo+16) = src;

	for(j=0;j<20;j++){
		k = (j==11)|(j==19);
		printf("%02x%c", tmp[j], k*(0xa-0x20)+0x20);
	}
}
void reply_arp(u8* buf, int len, u8* tmp, int max)
{
	printf("build arp\n");
	tmp[0] = 0;
	tmp[1] = 1;
	tmp[2] = 8;
	tmp[3] = 0;
	tmp[4] = 6;
	tmp[5] = 4;
	tmp[6] = 0;
	tmp[7] = 2;
	tmp[8] = 0xaa;
	tmp[9] = 0xbb;
	tmp[10] = 0xcc;
	tmp[11] = 0xdd;
	tmp[12] = 0xee;
	tmp[13] = 0xff;
	tmp[14] = 192;
	tmp[15] = 168;
	tmp[16] = 42;
	tmp[17] = 42;
	tmp[18] = buf[6];
	tmp[19] = buf[7];
	tmp[20] = buf[8];
	tmp[21] = buf[9];
	tmp[22] = buf[0xa];
	tmp[23] = buf[0xb];
	tmp[24] = 192;
	tmp[25] = 168;
	tmp[26] = 42;
	tmp[27] = 1;

	int j,k;
	for(j=0;j<28;j++){
		k = (j==7)|(j==7+10)|(j==7+20);
		printf("%02x%c", tmp[j], k*(0xa-0x20)+0x20);
	}
}
void reply_ether(u8* buf, int len, u8* tmp, int max)
{
	printf("build eth\n");
	tmp[0] = buf[6];
	tmp[1] = buf[7];
	tmp[2] = buf[8];
	tmp[3] = buf[9];
	tmp[4] = buf[0xa];
	tmp[5] = buf[0xb];
	tmp[6] = 0xaa;
	tmp[7] = 0xbb;
	tmp[8] = 0xcc;
	tmp[9] = 0xdd;
	tmp[10] = 0xee;
	tmp[11] = 0xff;
	tmp[12] = buf[12];
	tmp[13] = buf[13];

	int j,k;
	for(j=0;j<14;j++){
		k = (j==13);
		printf("%02x%c", tmp[j], k*(0xa-0x20)+0x20);
	}
}
void reply_packet(int fd, int flag, u8* buf, int len)
{
	u8 tmp[128];
	int type = (buf[12]<<8) | buf[13];
	if(type == 0x0806){
		printf("arp reply{\n");

		reply_arp(buf, len, tmp+14, 64-14);
		reply_ether(buf, len, tmp, 14);

		write(fd, tmp, 28+14);
		printf("}\n");
	}
	if(type == 0x0800){
		if(buf[14+9] == 1){
			printf("icmp reply{\n");
			reply_icmp(buf, len, tmp+14+20, 128-14-20);
			reply_ip(buf, len, tmp+14, 128-14);
			reply_ether(buf, len, tmp, 14);
			write(fd, tmp, len);
			printf("}\n");
		}
	}
}




int main(){
	int tunfd, nread, err;
	tunfd = open("/dev/net/tun", O_RDWR);
	if(tunfd < 0){
		printf("open error\n");
		return 0;
	}

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP|IFF_NO_PI;

	err = ioctl(tunfd, TUNSETIFF, (void*)&ifr);
	if(err < 0){
		printf("ioctl error %d\n", errno);
		goto byebye;
	}

	printf("sudo ifconfig %s up\n", ifr.ifr_name);
	printf("sudo ifconfig %s 192.168.42.1\n", ifr.ifr_name);

	char buffer[1500];
	while(1){
		nread = read(tunfd, buffer, sizeof(buffer));
		if(nread < 0){
			printf("read error\n");
			goto byebye;
		}
		print_packet(buffer, nread);
		reply_packet(tunfd, 0, buffer, nread);
	}

byebye:
	close(tunfd);
	return 0;
}
