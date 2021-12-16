#include <stdio.h>
#include <stdlib.h>




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




int parseh264_sei(unsigned char* buf, int len)
{
	return 0;
}
int parseh264_sps(unsigned char* buf, int len)
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
int parseh264_pps(unsigned char* buf, int len)
{
	return 0;
}
int parseh264(unsigned char* buf, int len)
{
	switch(buf[0]&0x1f){
	case 1:
		printf("slice\n");
		break;
	case 2:
		printf("dpa\n");
		break;
	case 3:
		printf("dpb\n");
		break;
	case 4:
		printf("dpc\n");
		break;
	case 5:
		printf("idr\n");
		break;
	case 6:
		printf("sei\n");
		parseh264_sei(buf, len);
		break;
	case 7:
		printf("sps\n");
		parseh264_sps(buf, len);
		break;
	case 8:
		printf("pps\n");
		parseh264_pps(buf, len);
		break;
	case 9:
		printf("aud\n");
		break;
	case 10:
		printf("end of sequence\n");
		break;
	case 11:
		printf("end of stream\n");
		break;
	case 12:
		printf("filter\n");
		break;
	case 13:
		printf("sps-ext\n");
		break;
	case 14:
		printf("prefix\n");
		break;
	case 15:
		printf("sps-sub\n");
		break;
	case 16:
		printf("dps\n");
		break;
	}
	return 0;
}
