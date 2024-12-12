#include <stdio.h>

int parseh266(unsigned char* buf, int len)
{
	int forbid = buf[0]>>7;
	//int reserved = (buf[0]>>6)&1;
	int layer = buf[0]&0x3f;
	int unittype = buf[1]>>3;
	int temporal = buf[1]&0x7;
	switch(unittype){
	case 0:
		printf("TRAIL{\n");
		break;
	case 1:
		printf("STSA{\n");
		break;
	case 2:
		printf("RADL{\n");
		break;
	case 3:
		printf("RASL{\n");
		break;
	case 4:
		printf("RSV_VCL_4{\n");
		break;
	case 5:
		printf("RSV_VCL_5{\n");
		break;
	case 6:
		printf("RSV_VCL_6{\n");
		break;
	case 7:
		printf("IDR_W_RADL{\n");
		break;
	case 8:
		printf("IDR_N_LP{\n");
		break;
	case 9:
		printf("CRA{\n");
		break;
	case 10:
		printf("GDR{\n");
		break;
	case 11:
		printf("RSV_IRAP_11{\n");
		break;
	case 12:
		printf("OPI{\n");
		break;
	case 13:
		printf("DCI{\n");
		break;
	case 14:
		printf("VPS{\n");
		break;
	case 15:
		printf("SPS{\n");
		break;
	case 16:
		printf("PPS{\n");
		break;
	case 17:
		printf("PREFIX_APS{\n");
		break;
	case 18:
		printf("SUFFIX_APS{\n");
		break;
	case 19:
		printf("PH{\n");
		break;
	case 20:
		printf("AUD{\n");
		break;
	case 21:
		printf("EOS{\n");
		break;
	case 22:
		printf("EOB{\n");
		break;
	case 23:
		printf("PREFIX_SEI{\n");
		break;
	case 24:
		printf("SUFFIX_SEI{\n");
		break;
	case 25:
		printf("FD{\n");
		break;
	case 26:
		printf("RSV_NVCL_26{\n");
		break;
	case 27:
		printf("RSV_NVCL_27{\n");
		break;
	case 28:
	case 29:
	case 30:
	case 31:
		printf("UNSPEC_%d{\n", unittype);
		break;
	default:
		printf("unknown_%x{\n", unittype);
	}
	printf("}\n\n");
}
