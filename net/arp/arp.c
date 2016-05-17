#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/if_arp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arppacket{         //自己构建的arp包，不含以太网头部
        unsigned short ar_hrd;          //arp硬件类型
        unsigned short ar_pro;          //arp协议
        unsigned char  ar_hln;          //arp硬件地址长度
        unsigned char  ar_pln;          //arp协议地址长度
        unsigned short ar_op;           //arp 操作码 //上面部分也可以用，struct arphdr结构来构造。 
        unsigned char ar_sha[ETH_ALEN];//发送方mac地址
        unsigned char ar_sip[4];        //放送方ip地址
        unsigned char ar_dha[ETH_ALEN]; //目的mac地址
        unsigned char ar_dip[4];        //目的ip地址 //上面是构造arp数据部分，用于以太网。
                 char pad[18];          //18个填充字节，满足以太网最少帧长度64字节含4字节校验码。
};

int main(int argc,char *argv[]){
        char ef[ETH_FRAME_LEN];
        struct ethhdr *p_ethhdr;     //以太网头部指针
        char eth_dest[ETH_ALEN]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};      //目的mac地址
        char eth_source[ETH_ALEN]={0x00,0x12,0x34,0x56,0x78,0x90};    //自己构造源mac地址与实际的不相符
        char eth_arpmac[ETH_ALEN]={0x00,0x00,0x00,0x00,0x00,0x00};
        int n;
        int fd;
        int i;
        if(argc!=3){
                printf("usage:%s sourceip destip\n",argv[0]);
                exit(1);
        }
        fd=socket(AF_INET,SOCK_PACKET,htons(ETH_P_ARP));  //
        p_ethhdr=(struct ethhdr *)ef;
        memcpy(p_ethhdr->h_dest,eth_dest,ETH_ALEN);
        memcpy(p_ethhdr->h_source,eth_source,ETH_ALEN);
        p_ethhdr->h_proto=htons(0x0806) ;//设置以太网头部的协议类型为0x0806也就是arp协议。
        struct arppacket *p_arp;
        struct in_addr in;
        p_arp=ef+ETH_HLEN;
        p_arp->ar_hrd=htons(0x01);  //arp硬件类型0x01为以太网类型
        p_arp->ar_pro=htons(0x0800); //设置协议类型0x0800表示ip网路
        p_arp->ar_hln=6;             //以太网地址长度为六
        p_arp->ar_pln=4;             //ip地址长度为4
        p_arp->ar_op=htons(ARPOP_REQUEST);
        memcpy(p_arp->ar_sha,eth_source,ETH_ALEN);
        memcpy(p_arp->ar_dha,eth_arpmac,ETH_ALEN);
        if((in.s_addr=inet_addr(argv[1]))==-1){
                perror("inet_addr");
                exit(1);
        }
        memcpy(p_arp->ar_sip,&in.s_addr,4);
        if((in.s_addr=inet_addr(argv[2]))==-1){
                perror("inet_addr");
                exit(1);
        }
         memcpy(p_arp->ar_dip,&in.s_addr,4);
        bzero(p_arp->pad,18);
        struct sockaddr to;                                      
        strcpy(to.sa_data,"eth0");
        for(i=0;i<8;i++){
                if(sendto(fd,&ef,64,0,&to,sizeof(to))<0){
                        perror("sendto");
                        exit(1);
                }
                sleep(10);
       }
        close(fd);
        return 0;
}
