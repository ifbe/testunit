#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;




void parseh264_printhex(u8* buf, int len)
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




int h264_u(unsigned char* buf, int* pos, int cnt)
{
	int j,aa,bb;
	int ret = 0;
//printf("@h264_u:bitpos=%x,cnt=%x\n", *pos, cnt);
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
//printf("@h264_ue+++:bitpos=%x,cnt=%d\n", *pos, cnt);
	while(1){
		aa = (*pos)/8;
		bb = (*pos)%8;
		if((buf[aa] & (0x80>>bb)) != 0)break;
		cnt += 1;
		*pos += 1;
	}
	ret = h264_u(buf, pos, cnt+1) - 1;

//printf("@h264_ue---:bitpos=%x,cnt=%d\n", *pos, cnt);
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




int parseh264_sei_user(unsigned char* buf, int len)
{
	printf("uuid=\n");
	parseh264_printhex(buf, 16);

	printf("str=\n");
	printf("%.*s\n", len-16, buf+16);
	return 0;
}
int parseh264_sei(unsigned char* buf, int len)
{
printf("sei{\n");
	int tmp;
	int old = 1;
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
		parseh264_sei_user(buf+old, size);
		break;
	default:
		now = old + size;
		printf(".[%x,%x)payload\n",old,now);
		parseh264_printhex(buf+old, size);
	}

printf("}//sei\n");
	return 0;
}



/*
void scaling_list(u32* scalinglist, int sizeofscalinglist, u32 usedefaultscalingmatrixflag)
{
	int lastscale = 8;
	int nextscale = 8;
	int j;
	for(j=0;j<sizeofscalinglist;j++){
		if(nextscale != 0){
			int delta_scale = h264_se();
			nextscale = (lastscale+delat_scale+256)%256;
			usedefaultscalingmatrixflag = (j==0 && nextscale==0);
		}
		scalinglist[j] = (nextscale==0)?lastscale:nextscale;
		lastscale = scalinglist[j];
	}
}*/




int parseh264_sps_vui_hrd(unsigned char* buf, int bitpos)
{
printf("sps_vui_hrd{\n");

	int cpb_cnt_minus1 = h264_ue(buf, &bitpos);
	printf("cpb_cnt_minus1 = %x\n", cpb_cnt_minus1);

	int bit_rate_scale = h264_u(buf, &bitpos, 4);
	printf("bit_rate_scale = %x\n", bit_rate_scale);

	int cpb_size_scale = h264_u(buf, &bitpos, 4);
	printf("cpb_size_scale = %x\n", cpb_size_scale);

	int j;
	for(j=0;j<cpb_cnt_minus1;j++){
		int bit_rate_value_minus1 = h264_ue(buf, &bitpos);
		int cpb_size_value_minus1 = h264_ue(buf, &bitpos);
		int cbr_flag = h264_u(buf, &bitpos, 1);
		printf("%d:bitrate=%x,cpbsize=%x,cbrflag=%x\n", j, bit_rate_value_minus1, cpb_size_value_minus1, cbr_flag);
	}

	int initial_cpb_removal_delay_length_minus1 = h264_u(buf, &bitpos, 5);
	printf("initial_cpb_removal_delay_length_minus1 = %x\n", initial_cpb_removal_delay_length_minus1);
	int cpb_removal_delay_length_minus1 = h264_u(buf, &bitpos, 5);
	printf("cpb_removal_delay_length_minus1 = %x\n", cpb_removal_delay_length_minus1);
	int dpb_output_delay_length_minus1 = h264_u(buf, &bitpos, 5);
	printf("dpb_output_delay_length_minus1 = %x\n", dpb_output_delay_length_minus1);
	int time_offset_length = h264_u(buf, &bitpos, 5);
	printf("time_offset_length = %x\n", time_offset_length);

printf("}//sps_vui_hrd\n");
	return bitpos;
}
int parseh264_sps_vui(unsigned char* buf, int bitpos)
{
printf("sps_vui{\n");

	int aspect_ratio_info_present_flag = h264_u(buf, &bitpos, 1);
	printf("aspect_ratio_info_present_flag = %x\n", aspect_ratio_info_present_flag);
	if(aspect_ratio_info_present_flag){
		int aspect_ratio_idc = h264_u(buf, &bitpos, 8);
		printf("aspect_ratio_idc = %x\n", aspect_ratio_idc);
		if(255 == aspect_ratio_idc){
			int sar_width = h264_u(buf, &bitpos, 16);
			printf("sar_width = %x\n", sar_width);

			int sar_height = h264_u(buf, &bitpos, 16);
			printf("sar_height = %x\n", sar_height);
		}
	}

	int overscan_info_present_flag = h264_u(buf, &bitpos, 1);
	printf("overscan_info_present_flag = %x\n", overscan_info_present_flag);
	if(overscan_info_present_flag){
		int overscan_appropriate_flag = h264_u(buf, &bitpos, 1);
		printf("overscan_appropriate_flag = %x\n", overscan_appropriate_flag);
	}

	int video_signal_type_present_flag = h264_u(buf, &bitpos, 1);
	printf("video_signal_type_present_flag = %x\n", video_signal_type_present_flag);
	if(video_signal_type_present_flag){
		int video_format = h264_u(buf, &bitpos, 3);
		printf("video_format = %x\n", video_format);

		int video_full_range_flag = h264_u(buf, &bitpos, 1);
		printf("video_full_range_flag = %x\n", video_full_range_flag);

		int colour_description_present_flag = h264_u(buf, &bitpos, 1);
		printf("colour_description_present_flag = %x\n", colour_description_present_flag);
		if(colour_description_present_flag){
			int colour_primaries = h264_u(buf, &bitpos, 8);
			printf("colour_primaries = %x\n", colour_primaries);
			int transfer_characteristics = h264_u(buf, &bitpos, 8);
			printf("transfer_characteristics = %x\n", transfer_characteristics);
			int marix_coefficients = h264_u(buf, &bitpos, 8);
			printf("marix_coefficients = %x\n", marix_coefficients);
		}
	}

	int chrome_loc_info_present_flag = h264_u(buf, &bitpos, 1);
	printf("chrome_loc_info_present_flag = %x\n", chrome_loc_info_present_flag);
	if(chrome_loc_info_present_flag){
		int chroma_sample_loc_type_top_field = h264_ue(buf, &bitpos);
		int chroma_sample_loc_type_bottom_field = h264_ue(buf, &bitpos);
	}

	int timing_info_present_flag = h264_u(buf, &bitpos, 1);
	printf("timing_info_present_flag = %x\n", timing_info_present_flag);
	if(timing_info_present_flag){
		u32 num_units_in_tick = h264_u(buf, &bitpos, 32);
		printf("num_units_in_tick = %x\n", num_units_in_tick);
		u32 time_scale = h264_u(buf, &bitpos, 32);
		printf("time_scale = %x\n", time_scale);
		int fixed_frame_rate_flag = h264_u(buf, &bitpos, 1);
		printf("fixed_frame_rate_flag = %x\n", fixed_frame_rate_flag);
	}

	int nal_hrd_parameters_present_flag = h264_u(buf, &bitpos, 1);
	printf("nal_hrd_parameters_present_flag = %x\n", nal_hrd_parameters_present_flag);
	if(nal_hrd_parameters_present_flag){
		bitpos = parseh264_sps_vui_hrd(buf, bitpos);
	}

	int vcl_hrd_parameters_present_flag = h264_u(buf, &bitpos, 1);
	printf("vcl_hrd_parameters_present_flag = %x\n", vcl_hrd_parameters_present_flag);
	if(vcl_hrd_parameters_present_flag){
		bitpos = parseh264_sps_vui_hrd(buf, bitpos);
	}

	if(nal_hrd_parameters_present_flag | vcl_hrd_parameters_present_flag){
		int low_delay_hrd_flag = h264_u(buf, &bitpos, 1);
		printf("low_delay_hrd_flag = %x\n", low_delay_hrd_flag);
	}

	int pic_struct_present_flag = h264_u(buf, &bitpos, 1);
	printf("pic_struct_present_flag = %x\n", pic_struct_present_flag);

	int bitstream_restriction_flag = h264_u(buf, &bitpos, 1);
	printf("bitstream_restriction_flag = %x\n", bitstream_restriction_flag);
	if(bitstream_restriction_flag){
		int motion_vectors_over_pic_boundaries_flag = h264_u(buf, &bitpos, 1);
		printf("motion_vectors_over_pic_boundaries_flag = %x\n",motion_vectors_over_pic_boundaries_flag);

		int max_bypes_per_pic_danom = h264_ue(buf, &bitpos);
		printf("max_bypes_per_pic_danom = %x\n", max_bypes_per_pic_danom);
		int max_bits_per_mb_danom = h264_ue(buf, &bitpos);
		printf("max_bits_per_mb_danom = %x\n", max_bits_per_mb_danom);
		int log2_max_mv_length_horizontal = h264_ue(buf, &bitpos);
		printf("log2_max_mv_length_horizontal = %x\n", log2_max_mv_length_horizontal);
		int log2_max_mv_length_vertical = h264_ue(buf, &bitpos);
		printf("log2_max_mv_length_vertical = %x\n", log2_max_mv_length_vertical);
		int num_reorder_frames = h264_ue(buf, &bitpos);
		printf("num_reorder_frames = %x\n", num_reorder_frames);
		int max_dec_frame_buffering = h264_ue(buf, &bitpos);
		printf("max_dec_frame_buffering = %x\n", max_dec_frame_buffering);
	}
printf("}//sps_vui\n");
	return bitpos;
}
int parseh264_sps(unsigned char* buf, int len)
{
printf("sps{\n");
	int j;
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
		printf("chroma_format_idc = %x\n", chroma_format_idc);
		if(3 == chroma_format_idc){
			int seperate_colour_plane_flag = h264_u(buf, &bitpos, 1);
			printf("seperate_colour_plane_flag = %x\n", seperate_colour_plane_flag);
		}

		int bit_depth_luma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_luma_minus8 = %x\n",bit_depth_luma_minus8);

		int bit_depth_chroma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_chroma_minus8 = %x\n",bit_depth_chroma_minus8);

		int qp_prime_y_zero_transform_bypass_flag = h264_u(buf, &bitpos, 1);
		printf("qp_prime_y_zero_transform_bypass_flag = %x\n",qp_prime_y_zero_transform_bypass_flag);

		int seq_scaling_matrix_present_flag = h264_u(buf, &bitpos, 1);
		printf("seq_scaling_matrix_present_flag = %x\n",seq_scaling_matrix_present_flag);
		if(seq_scaling_matrix_present_flag){
			u8 seq_scaling_list_present_flag[16];
			for(j=0;j<((chroma_format_idc != 3)?8:12);j++){
				seq_scaling_list_present_flag[j] = h264_u(buf, &bitpos, 1);
				if(j < 6){
					//scaling_list(scalinglist4x4[j], 16, usedefaultscalingmatrix4x4flag[j]);
				}
				else{
					//scaling_list(scalinglist8x8[j-6], 64, usedefaultscalingmatrix8x8flag[j-6]);
				}
			}
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
		int delta_pic_order_always_zero_flag = h264_u(buf, &bitpos, 1);
		int offset_for_non_ref_pic = h264_se(buf, &bitpos);
		int offset_for_top_to_bottom_field = h264_se(buf, &bitpos);
		int num_ref_frames_in_pic_order_cnt_cycle = h264_ue(buf, &bitpos);

		u8 offset_for_ref_frame[16];
		for(j=0;j<num_ref_frames_in_pic_order_cnt_cycle;j++){
			offset_for_ref_frame[j] = h264_se(buf, &bitpos);
		}
	}

	int max_num_ref_frames = h264_ue(buf, &bitpos);
	printf("max_num_ref_frames = %x\n", max_num_ref_frames);

	int gaps_in_frame_num_value_allowed_flag = h264_u(buf, &bitpos, 1);
	printf("gaps_in_frame_num_value_allowed_flag = %x\n", gaps_in_frame_num_value_allowed_flag);

	int pic_width_in_mbs_minus1 = h264_ue(buf, &bitpos);
	printf("pic_width_in_mbs_minus1 = %x(%d)\n", pic_width_in_mbs_minus1, (pic_width_in_mbs_minus1+1)*16);

	int pic_height_in_map_units_minus1 = h264_ue(buf, &bitpos);
	printf("pic_height_in_map_units_minus1 = %x(%d)\n", pic_height_in_map_units_minus1, (pic_height_in_map_units_minus1+1)*16);

	int frame_mbs_only_flag = h264_u(buf, &bitpos, 1);
	printf("frame_mbs_only_flag = %x\n", frame_mbs_only_flag);
	if(!frame_mbs_only_flag){
		int mb_adaptive_frame_field_flag = h264_u(buf, &bitpos, 1);
		printf("mb_adaptive_frame_field_flag = %x\n", mb_adaptive_frame_field_flag);
	}

	int direct_8x8_inference_flag = h264_u(buf, &bitpos, 1);
	printf("direct_8x8_inference_flag = %x\n", direct_8x8_inference_flag);

	int frame_cropping_flag = h264_u(buf, &bitpos, 1);
	printf("frame_cropping_flag = %x\n", frame_cropping_flag);
	if(frame_cropping_flag){
		int frame_crop_left_offset = h264_ue(buf, &bitpos);
		printf("frame_crop_left_offset = %x\n", frame_crop_left_offset);
		int frame_crop_right_offset = h264_ue(buf, &bitpos);
		printf("frame_crop_right_offset = %x\n", frame_crop_right_offset);
		int frame_crop_top_offset = h264_ue(buf, &bitpos);
		printf("frame_crop_top_offset = %x\n", frame_crop_top_offset);
		int frame_crop_bottom_offset = h264_ue(buf, &bitpos);
		printf("frame_crop_bottom_offset = %x\n", frame_crop_bottom_offset);
	}

	int vui_parameters_present_flag = h264_u(buf, &bitpos, 1);
	printf("vui_parameters_present_flag = %x\n", vui_parameters_present_flag);
	if(vui_parameters_present_flag){
		parseh264_sps_vui(buf, bitpos);
	}
printf("}//sps\n");
	return 0;
}
int parseh264_pps(unsigned char* buf, int len)
{
printf("pps{\n");
	int j;
	int bitpos = 8;

	int pic_parameter_set_id = h264_ue(buf, &bitpos);
	printf("pic_parameter_set_id = %x\n", pic_parameter_set_id);

	int seq_parameter_set_id = h264_ue(buf, &bitpos);
	printf("seq_parameter_set_id = %x\n", seq_parameter_set_id);

	int entropy_coding_mode_flag = h264_u(buf, &bitpos, 1);
	printf("entropy_coding_mode_flag = %x\n", entropy_coding_mode_flag);

	int bottom_field_pic_order_in_frame_present_flag = h264_u(buf, &bitpos, 1);
	printf("bottom_field_pic_order_in_frame_present_flag = %x\n",bottom_field_pic_order_in_frame_present_flag);
	int num_slice_groups_minus1 = h264_ue(buf, &bitpos);
	printf("num_slice_groups_minus1 = %x\n", num_slice_groups_minus1);
	if(num_slice_groups_minus1 > 0){
		int slice_group_map_type = h264_ue(buf, &bitpos);
		printf("slice_group_map_type = %x\n", slice_group_map_type);

		int pic_size_in_map_units_minus1;
		switch(slice_group_map_type){
		case 0:
			for(j=0;j<num_slice_groups_minus1;j++){
				printf("%d:run_length_minus1 = %x\n", j, h264_ue(buf, &bitpos));
			}
			break;
		case 2:
			for(j=0;j<num_slice_groups_minus1;j++){
				printf("%d:top_left = %x\n", j, h264_ue(buf, &bitpos));
				printf("%d:run_length_minus1 = %x\n", j, h264_ue(buf, &bitpos));
			}
			break;
		case 3:
		case 4:
		case 5:
			printf("slice_group_change_direction_flag = %x\n", h264_u(buf, &bitpos, 1));
			printf("slice_group_change_rate_minus1 = %x\n", h264_ue(buf, &bitpos));
			break;
		case 6:
			pic_size_in_map_units_minus1 = h264_ue(buf, &bitpos);
			printf("pic_size_in_map_units_minus1 = %x\n", pic_size_in_map_units_minus1);
			for(j=0;j<pic_size_in_map_units_minus1;j++){
				printf("%d:slice_group_id = %x\n", j, h264_ue(buf, &bitpos));	//todo: it is u(v)
			}
			break;
		}
	}

	int num_ref_idx_10_default_active_minus1 = h264_ue(buf, &bitpos);
	printf("num_ref_idx_10_default_active_minus1 = %x\n", num_ref_idx_10_default_active_minus1);
	int num_ref_idx_11_default_active_minus1 = h264_ue(buf, &bitpos);
	printf("num_ref_idx_11_default_active_minus1 = %x\n", num_ref_idx_11_default_active_minus1);

	int weighted_pred_flag = h264_u(buf, &bitpos, 1);
	printf("weighted_pred_flag = %x\n", weighted_pred_flag);
	int weighted_bipred_idc = h264_u(buf, &bitpos, 2);
	printf("weighted_bipred_idc = %x\n", weighted_bipred_idc);

	int pic_init_qp_minus26 = h264_se(buf, &bitpos);
	printf("pic_init_qp_minus26 = %x\n", pic_init_qp_minus26);
	int pic_init_qs_minus26 = h264_se(buf, &bitpos);
	printf("pic_init_qs_minus26 = %x\n", pic_init_qs_minus26);

	int chroma_qp_index_offset = h264_se(buf, &bitpos);
	printf("chroma_qp_index_offset = %x\n", chroma_qp_index_offset);

	int deblocking_filter_control_present_flag = h264_u(buf, &bitpos, 1);
	printf("deblocking_filter_control_present_flag = %x\n", deblocking_filter_control_present_flag);
	int constrained_intra_pred_flag = h264_u(buf, &bitpos, 1);
	printf("constrained_intra_pred_flag = %x\n", constrained_intra_pred_flag);
	int redundant_pic_cnt_present_flag = h264_u(buf, &bitpos, 1);
	printf("redundant_pic_cnt_present_flag = %x\n", redundant_pic_cnt_present_flag);

	if(bitpos >= (len+2)*8)goto done;

	int transform_8x8_mode_flag = h264_u(buf, &bitpos, 1);
	printf("transform_8x8_mode_flag = %x\n", transform_8x8_mode_flag);

	int pic_scaling_matrix_present_flag = h264_u(buf, &bitpos, 1);
	printf("pic_scaling_matrix_present_flag = %x\n", pic_scaling_matrix_present_flag);
/*
	if(pic_scaling_matrix_present_flag){
		int xxxx = 6+transform_8x8_mode_flag*((chroma_format_idc!=3)?2:6);
		int pic_scaling_list_present_flag;
		for(j=0;j<xxxx;j++){
			pic_scaling_list_present_flag = h264_u(buf, &bitpos, 1);
			printf("pic_scaling_list_present_flag = %x\n", pic_scaling_list_present_flag);
			if(pic_scaling_list_present_flag){
				if(j<6){
					//scaling_list(scalinglist4x4[j], 16, usedefaultscalingmatrix4x4flag[j]);
				}
				else{
					//scaling_list(scalinglist8x8[j-6], 64, usedefaultscalingmatrix8x8flag[j-6]);
				}
			}
		}
	}
*/
done:
printf("}//pps\n");
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
		parseh264_sei(buf, len);
		break;
	case 7:
		parseh264_sps(buf, len);
		break;
	case 8:
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
