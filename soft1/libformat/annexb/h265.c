#include <stdio.h>
#include <stdlib.h>




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
		break;
	case 0x28:
		printf("SEI_SUFFIX\n");
		break;
	}
	return 0;
}
