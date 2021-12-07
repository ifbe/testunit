#include <stdio.h>
#include <stdlib.h>


static unsigned char buf[0x100000];


int find0001(unsigned char* ptr, int len)
{
	int j;
	for(j=4;j<len;j++){
		if(	(0 == buf[j+0])&&
			(0 == buf[j+1])&&
			(0 == buf[j+2])&&
			(1 == buf[j+3]) )
		{
			return j;
		}
	}
	return -1;
}
int ebsp2rbsp(unsigned char* ptr, int len, unsigned char* dst, int max)
{
	if(0 != ptr[0])return -1;
	if(0 != ptr[1])return -2;
	if(2 <= ptr[2])return -3;

	else if(1 == ptr[2]){
		ptr += 3;
		len -= 3;
	}
	else if(0 == ptr[2]){
		if(1 != ptr[3])return -4;
		ptr +=4;
		len -=4;
	}


	int j,k=0;
	for(j=0;j<len-2;j++){
		if((0 == ptr[j])&&(0 == ptr[j+1])&&(3 == ptr[j+2])){
			printf("003@%x\n",j);
			dst[k+0] = 0;
			dst[k+1] = 0;
			k += 2;
			j += 2;
			continue;
		}
		dst[k] = ptr[j];
		k++;
	}
	dst[k] = ptr[j];
	dst[k+1] = ptr[j+1];
	k += 2;

	return k;
}




int h264_u(unsigned char* buf, int* pos, int cnt)
{
	int j,aa,bb;
	int ret = 0;
printf("@h264_u:bitpos=%x,cnt=%x\n", *pos, cnt);
	for(j=0;j<cnt;j++){
		ret = ret<<1;
		aa = (*pos)/8;
		bb = (*pos)%8;
		if((buf[aa] & (0x80>>bb)) != 0)ret |= 1;
		*pos += 1;
	}
	return ret;
}
int h264_ue(unsigned char* buf, int* pos)
{
	int aa,bb;
	int ret, cnt = 0;
printf("@h264_ue+++:bitpos=%x,cnt=%d\n", *pos, cnt);
	while(1){
		aa = (*pos)/8;
		bb = (*pos)%8;
		if((buf[aa] & (0x80>>bb)) != 0)break;
		cnt += 1;
		*pos += 1;
	}
	ret = h264_u(buf, pos, cnt+1) - 1;

printf("@h264_ue---:bitpos=%x,cnt=%d\n", *pos, cnt);
	return ret;
}
int h264_se(unsigned char* buf, int* pos)
{
	int ret;
	int aa,bb;
	int cnt = 0;
	while(1){
		aa = (*pos)/8;
		bb = (*pos)%8;
		if((buf[aa] & (0x80>>bb)) != 0)break;
		cnt += 1;
		*pos += 1;
	}

	ret = h264_u(buf, pos, cnt);

	aa = (*pos)/8;
	bb = (*pos)%8;
	if((buf[aa] & (0x80>>bb)) != 0)ret = -ret;

	return ret;
}




int parsesps(unsigned char* buf, int len)
{
	int profile_idc = buf[1];
	int level_idc = buf[3];
	printf("profile_idc = %x(%d)\n", profile_idc, profile_idc);
	printf("level_idc = %x(%d)\n", level_idc, level_idc);

	int bitpos = 32;
	int seq_parameter_set_id = h264_ue(buf, &bitpos);
	printf("seq_parameter_set_id = %x\n", seq_parameter_set_id);

	if(( 44 == profile_idc) |
	   ( 83 == profile_idc) |
	   ( 86 == profile_idc) |
	   (100 == profile_idc) |
	   (110 == profile_idc) |
	   (118 == profile_idc) |
	   (122 == profile_idc) |
	   (128 == profile_idc) |
	   (134 == profile_idc) |
	   (135 == profile_idc) |
	   (138 == profile_idc) |
	   (139 == profile_idc) |
	   (244 == profile_idc) )
	{
		int chroma_format_idc = h264_ue(buf, &bitpos);
		int seperate_colour_plane_flag = 0;
		if(3 == chroma_format_idc)seperate_colour_plane_flag = h264_u(buf, &bitpos, 1);
		printf("chroma_format_idc = %x,seperate_colour_plane_flag = %x\n",
			chroma_format_idc, seperate_colour_plane_flag);

		int bit_depth_luma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_luma_minus8 = %x\n",bit_depth_luma_minus8);
		int bit_depth_chroma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_chroma_minus8 = %x\n",bit_depth_chroma_minus8);

		int qp_prime_y_zero_transform_bypass_flag = h264_u(buf, &bitpos, 1);
		printf("qp_prime_y_zero_transform_bypass_flag = %x\n",qp_prime_y_zero_transform_bypass_flag);

		int seq_scaling_matrix_present_flag = h264_u(buf, &bitpos, 1);
		printf("seq_scaling_matrix_present_flag = %x\n",seq_scaling_matrix_present_flag);
		if(seq_scaling_matrix_present_flag){
		}
	}

	int log2_max_frame_num_minus4 = h264_ue(buf, &bitpos);
	printf("log2_max_frame_num_minus4 = %x\n", log2_max_frame_num_minus4);

	int pic_order_cnt_type = h264_ue(buf, &bitpos);
	printf("pic_order_cnt_type = %x\n", pic_order_cnt_type);
	if(0 == pic_order_cnt_type){
		int log2_max_pic_order_cnt_lsb_minus4 = h264_ue(buf, &bitpos);
		printf("log2_max_pic_order_cnt_lsb_minus4 = %x\n",log2_max_pic_order_cnt_lsb_minus4);
	}
	else if(1 == pic_order_cnt_type){
	}

	int max_num_ref_frames = h264_ue(buf, &bitpos);
	printf("max_num_ref_frames = %x\n", max_num_ref_frames);

	int gaps_in_frame_num_value_allowed_flag = h264_u(buf, &bitpos, 1);
	printf("gaps_in_frame_num_value_allowed_flag = %x\n", gaps_in_frame_num_value_allowed_flag);

	int pic_width_in_mbs_minus1 = h264_ue(buf, &bitpos);
	printf("pic_width_in_mbs_minus1 = %x(%d)\n", pic_width_in_mbs_minus1, (pic_width_in_mbs_minus1+1)*16);

	int pic_height_in_map_units_minus1 = h264_ue(buf, &bitpos);
	printf("pic_height_in_map_units_minus1 = %x(%d)\n", pic_height_in_map_units_minus1, (pic_height_in_map_units_minus1+1)*16);
	return 0;
}
int parserbsp(unsigned char* buf, int len)
{
	switch(buf[0]&0x1f){
	case 7:
		printf("sps\n");
		parsesps(buf, len);
		break;
	}
	return 0;
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out /path/to/xxx.h264\n");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");
	if(0 == fp)goto error;

	int ret,tmp;
	int now = 0;
	int cnt = 0;
	while(1){
		ret = fseek(fp, now, SEEK_SET);
		//if(ret < 0)

		ret = fread(buf, 1, 0x100000, fp);
		if(ret <= 0)break;

		ret = find0001(buf, ret);
		if(ret <= 0)break;

		printf("%x@[%x,%x]:%x,%x,%x,%x\n", cnt, now, now+ret, buf[4],buf[5],buf[6],buf[7]);
		tmp = ebsp2rbsp(buf, ret, buf, ret);
		printf("ebsp2rbsp:%x->%x\n", ret, tmp);
		parserbsp(buf, tmp);

		now += ret;
		cnt++;
	}
good:
	printf("cnt=0x%x(%d)\n", cnt, cnt);
	fclose(fp);
	return 0;

error:
	return -1;
}
