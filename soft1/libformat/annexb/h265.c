#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;




void parseh265_printhex(u8* buf, int len)
{
	int j;
	for(j=0;j<len;j++){
		if(j==len-1){
			printf("%02x\n", buf[j]);
			break;
		}

		if(15 == (j%16))printf("%02x\n", buf[j]);
		else printf("%02x ", buf[j]);
	}
}




int parseh265_sei_user(unsigned char* buf, int len)
{
	printf("uuid=\n");
	parseh265_printhex(buf, 16);

	printf("str=\n");
	printf("%.*s\n", len-16, buf+16);
	return 0;
}
int parseh265_sei(unsigned char* buf, int len)
{
	int tmp;
	int old = 2;
	int now = old;

	int type = 0;
	do{
		tmp = buf[now];
		type += tmp;
		now++;
	}while(0xff == tmp);
	printf(".[%x,%x)type=%x\n",old,now,type);

	old = now;
	int size = 0;
	do{
		tmp = buf[now];
		size += tmp;
		now++;
	}while(0xff == tmp);
	printf(".[%x,%x)size=%x\n",old,now,size);

	old = now;
	switch(type){
	case 5:
		parseh265_sei_user(buf+old, size);
		break;
	default:
		now = old + size;
		printf(".[%x,%x)payload\n",old,now);
		parseh265_printhex(buf+old, size);
	}

	return 0;
}




int parseh265(unsigned char* buf, int len)
{
	switch((buf[0]>>1)&0x3f){
	case 0x0:
		printf("SLICE_TRAIL_N\n");
		break;
	case 0x1:
		printf("SLICE_TRAIL_R\n");
		break;
	case 0x2:
		printf("SLICE_TSA_N\n");
		break;
	case 0x3:
		printf("SLICE_TSA_R\n");
		break;
	case 0x4:
		printf("SLICE_STSA_N\n");
		break;
	case 0x5:
		printf("SLICE_STSA_R\n");
		break;
	case 0x6:
		printf("SLICE_RADL_N\n");
		break;
	case 0x7:
		printf("SLICE_RADL_R\n");
		break;
	case 0x8:
		printf("SLICE_RASL_N\n");
		break;
	case 0x9:
		printf("SLICE_RASL_R\n");
		break;
	case 0x10:
		printf("SLICE_BLA_W_LP\n");
		break;
	case 0x11:
		printf("SLICE_BLA_W_RADL\n");
		break;
	case 0x12:
		printf("SLICE_BLA_N_LP\n");
		break;
	case 0x13:
		printf("SLICE_IDR_W_RADL\n");
		break;
	case 0x14:
		printf("SLICE_IDR_N_LP\n");
		break;
	case 0x15:
		printf("SLICE_CRA\n");
		break;
	case 0x20:
		printf("vps\n");
		break;
	case 0x21:
		printf("sps\n");
		break;
	case 0x22:
		printf("pps\n");
		break;
	case 0x23:
		printf("ACCESS_UNIT_DELIMITER\n");
		break;
	case 0x24:
		printf("EOS\n");
		break;
	case 0x25:
		printf("EOB\n");
		break;
	case 0x26:
		printf("FILLER_DATA\n");
		break;
	case 0x27:
		printf("SEI\n");
		parseh265_sei(buf, len);
		break;
	case 0x28:
		printf("SEI_SUFFIX\n");
		break;
	}
	return 0;
}
