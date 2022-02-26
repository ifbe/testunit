#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define _P 0
#define _B 1
#define _I 2
#define _SP 3
#define _SI 4
char* h264_typetable[5]={"P","B","I","SP","SI"};




int h264_log2(u32 x)
{
        return __builtin_clz(x|1);
}


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
	*pos += 1;

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

printf("}sei\n\n");
	return 0;
}



/*
scaling_list(scalingList, sizeOfScalingList,useDefaultScalingMatrixFlag ) {
	lastScale = 8
	nextScale = 8
	for( j = 0; j < sizeOfScalingList; j++ ) {
		if( nextScale != 0 ) {
			delta_scale     se(v)
			nextScale = ( lastScale + delta_scale + 256 ) % 256
			useDefaultScalingMatrixFlag = ( j = = 0 && nextScale = = 0 )
		}
		scalingList[ j ] = ( nextScale = = 0 ) ? lastScale : nextScale
		lastScale = scalingList[ j ]
	}
}*/
int scaling_list(
	u8* buf, int bitpos,
	int* scalinglist, int sizeofscalinglist, u32 usedefaultscalingmatrixflag)
{
	int lastscale = 8;
	int nextscale = 8;
	int j;
	for(j=0;j<sizeofscalinglist;j++){
		if(nextscale != 0){
			int delta_scale = h264_se(buf, &bitpos);
			nextscale = (lastscale+delta_scale+256)%256;
			usedefaultscalingmatrixflag = (j==0 && nextscale==0);
		}
		scalinglist[j] = (nextscale==0)?lastscale:nextscale;
		lastscale = scalinglist[j];
	}
	return bitpos;
}




/*
hrd_parameters( ) {
	cpb_cnt_minus1     ue(v)
	bit_rate_scale     u(4)
	cpb_size_scale     u(4)
	for( SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {
		bit_rate_value_minus1[ SchedSelIdx ]  ue(v)
		cpb_size_value_minus1[ SchedSelIdx ]  ue(v)
		cbr_flag[ SchedSelIdx ]   u(1)
	}
	initial_cpb_removal_delay_length_minus1         u(5)
	cpb_removal_delay_length_minus1   u(5)
	dpb_output_delay_length_minus1    u(5)
	time_offset_length     u(5)
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

printf("}sps_vui_hrd\n");
	return bitpos;
}




/*
vui_parameters( ) {
	aspect_ratio_info_present_flag   u(1)
	if( aspect_ratio_info_present_flag ) {
		aspect_ratio_idc   u(8)
		if( aspect_ratio_idc = = Extended_SAR ) {
			sar_width    u(16)
			sar_height   u(16)
		}
	}
	overscan_info_present_flag  u(1)
	if( overscan_info_present_flag )
		overscan_appropriate_flag   u(1)
	video_signal_type_present_flag   u(1)
	if( video_signal_type_present_flag ) {
		video_format   u(3)
		video_full_range_flag   u(1)
		colour_description_present_flag   u(1)
		if( colour_description_present_flag ) {
			colour_primaries   u(8)
			transfer_characteristics   u(8)
			matrix_coefficients    u(8)
		}
	}
	chroma_loc_info_present_flag   u(1)
	if( chroma_loc_info_present_flag ) {
		chroma_sample_loc_type_top_field   ue(v)
		chroma_sample_loc_type_bottom_field  ue(v)
	}
	timing_info_present_flag   u(1)
	if( timing_info_present_flag ) {
		num_units_in_tick   u(32)
		time_scale   u(32)
		fixed_frame_rate_flag   u(1)
	}
	nal_hrd_parameters_present_flag   u(1)
	if( nal_hrd_parameters_present_flag )
		hrd_parameters( )
	vcl_hrd_parameters_present_flag  u(1)
	if( vcl_hrd_parameters_present_flag )
		hrd_parameters( )
	if( nal_hrd_parameters_present_flag | | vcl_hrd_parameters_present_flag )
		low_delay_hrd_flag   u(1)
	pic_struct_present_flag   u(1)
	bitstream_restriction_flag   u(1)
	if( bitstream_restriction_flag ) {
		motion_vectors_over_pic_boundaries_flag           u(1)
		max_bytes_per_pic_denom    ue(v)
		max_bits_per_mb_denom     ue(v)
		log2_max_mv_length_horizontal    ue(v)
		log2_max_mv_length_vertical    ue(v)
		max_num_reorder_frames    ue(v)
		max_dec_frame_buffering    ue(v)
	}
}*/
struct parsed_vui{
	u32 aspect_ratio_info_present_flag;
	u32 aspect_ratio_idc;
	u32 sar_width;
	u32 sar_height;
	u32 overscan_info_present_flag;
	u32 overscan_appropriate_flag;
	u32 video_signal_type_present_flag;
	u32 video_format;
	u32 video_full_range_flag;
	u32 colour_description_present_flag;
	u32 colour_primaries;
	u32 transfer_characteristics;
	u32 matrix_coefficients;
	u32 chroma_loc_info_present_flag;
	u32 chroma_sample_loc_type_top_field;
	u32 chroma_sample_loc_type_bottom_field;
	u32 timing_info_present_flag;
	u32 num_units_in_tick;
	u32 time_scale;
	u32 fixed_frame_rate_flag;
	u32 nal_hrd_parameters_present_flag;
	//hrd_parameters( )
	u32 vcl_hrd_parameters_present_flag;
	//hrd_parameters( )
	u32 low_delay_hrd_flag;
	u32 pic_struct_present_flag;
	u32 bitstream_restriction_flag;
	u32 motion_vectors_over_pic_boundaries_flag;
	u32 max_bytes_per_pic_denom;
	u32 max_bits_per_mb_denom;
	u32 log2_max_mv_length_horizontal;
	u32 log2_max_mv_length_vertical;
	u32 max_num_reorder_frames;
	u32 max_dec_frame_buffering;
};
static struct parsed_vui pervui;
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
printf("}sps_vui\n");
	return bitpos;
}



/*
seq_parameter_set_data( ) {
	profile_idc    u(8)
	constraint_set0_flag    u(1)
	constraint_set1_flag    u(1)
	constraint_set2_flag    u(1)
	constraint_set3_flag    u(1)
	constraint_set4_flag    u(1)
	constraint_set5_flag    u(1)
	reserved_zero_2bits     u(2)
	level_idc           u(8)
	seq_parameter_set_id      ue(v)
	if(	profile_idc = = 100 |
		profile_idc = = 110 |
		profile_idc = = 122 |
		profile_idc = = 244 |
		profile_idc = = 44 |
		profile_idc = = 83 |
		profile_idc = = 86 |
		profile_idc = = 118 |
		profile_idc = = 128 |
		profile_idc = = 138 |
		profile_idc = = 139 |
		profile_idc = = 134 |
		profile_idc = = 135 )
	{
		chroma_format_idc    ue(v)
		if( chroma_format_idc = = 3 )
			separate_colour_plane_flag    u(1)
		bit_depth_luma_minus8    ue(v)
		bit_depth_chroma_minus8  ue(v)
		qpprime_y_zero_transform_bypass_flag   u(1)
		seq_scaling_matrix_present_flag   u(1)
		if( seq_scaling_matrix_present_flag ){
			for( i = 0; i < ( ( chroma_format_idc != 3 ) ? 8 : 12 ); i++ ) {
				seq_scaling_list_present_flag[ i ]   u(1)
				if( seq_scaling_list_present_flag[ i ] ){
					if( i < 6 ){
						scaling_list( ScalingList4x4[ i ], 16,  UseDefaultScalingMatrix4x4Flag[ i ] )
					}
					else{
						scaling_list( ScalingList8x8[ i − 6 ], 64, UseDefaultScalingMatrix8x8Flag[ i − 6 ] )
					}
				}
			}
		}
	}

	log2_max_frame_num_minus4    ue(v)
	pic_order_cnt_type    ue(v)
	if( pic_order_cnt_type = = 0 ){
		log2_max_pic_order_cnt_lsb_minus4   ue(v)
	}
	else if( pic_order_cnt_type = = 1 ) {
		delta_pic_order_always_zero_flag   u(1)
		offset_for_non_ref_pic   se(v)
		offset_for_top_to_bottom_field    se(v)
		num_ref_frames_in_pic_order_cnt_cycle   ue(v)
		for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ ){
			offset_for_ref_frame[ i ]   se(v)
		}
	}
	max_num_ref_frames   ue(v)
	gaps_in_frame_num_value_allowed_flag   u(1)
	pic_width_in_mbs_minus1   ue(v)
	pic_height_in_map_units_minus1   ue(v)
	frame_mbs_only_flag   u(1)
	if( !frame_mbs_only_flag ){
		mb_adaptive_frame_field_flag   u(1)
	}
	direct_8x8_inference_flag   u(1)

	frame_cropping_flag   u(1)
	if( frame_cropping_flag ) {
		frame_crop_left_offset    ue(v)
		frame_crop_right_offset   ue(v)
		frame_crop_top_offset   ue(v)
		frame_crop_bottom_offset   ue(v)
	}

	vui_parameters_present_flag   u(1)
	if( vui_parameters_present_flag ){
		vui_parameters( )
	}
}*/
struct sps_parsed{
	u32 profile_idc;
	u32 constraint_set_flag;
	u32 level_idc;
	u32 seq_parameter_set_id;
	u32 chroma_format_idc;
	u32 separate_colour_plane_flag;
	u32 bit_depth_luma_minus8;
	u32 bit_depth_chroma_minus8;
	u32 qpprime_y_zero_transform_bypass_flag;
	u32 seq_scaling_matrix_present_flag;
	u32 seq_scaling_list_present_flag[8];
	u32 ScalingList4x4[16][16];
	u32 UseDefaultScalingMatrix4x4Flag[16];
	u32 ScalingList8x8[16][16];
	u32 UseDefaultScalingMatrix8x8Flag[16];
	u32 log2_max_frame_num;
	u32 pic_order_cnt_type;
	u32 log2_max_pic_order_cnt_lsb;
	u32 delta_pic_order_always_zero_flag;
	u32 offset_for_non_ref_pic;
	u32 offset_for_top_to_bottom_field;
	u32 num_ref_frames_in_pic_order_cnt_cycle;
	u32 offset_for_ref_frame[8];
	u32 max_num_ref_frames;
	u32 gaps_in_frame_num_value_allowed_flag;
	u32 pic_width_in_mbs_minus1;
	u32 pic_height_in_map_units_minus1;
	u32 frame_mbs_only_flag;
	u32 mb_adaptive_frame_field_flag;
	u32 direct_8x8_inference_flag;

	u32 frame_cropping_flag;
	u32 frame_crop_left_offset;
	u32 frame_crop_right_offset;
	u32 frame_crop_top_offset;
	u32 frame_crop_bottom_offset;

	u32 vui_parameters_present_flag;
};
static struct sps_parsed persps[16]={};
static int sps_status[16]={};
void* setsps(int id)
{
	return &persps[id];
}
void* getsps(int id)
{
	return &persps[id];
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

	struct sps_parsed* thesps = setsps(seq_parameter_set_id);
	if(0 == thesps){
		printf("error:cannot setsps\n");
		return 0;
	}

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
		thesps->chroma_format_idc = h264_ue(buf, &bitpos);
		printf("chroma_format_idc = %x\n", thesps->chroma_format_idc);
		if(3 == thesps->chroma_format_idc){
			thesps->separate_colour_plane_flag = h264_u(buf, &bitpos, 1);
			printf("separate_colour_plane_flag = %x\n", thesps->separate_colour_plane_flag);
		}

		int bit_depth_luma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_luma_minus8 = %x\n",bit_depth_luma_minus8);

		int bit_depth_chroma_minus8 = h264_ue(buf, &bitpos);
		printf("bit_depth_chroma_minus8 = %x\n",bit_depth_chroma_minus8);

		int qp_prime_y_zero_transform_bypass_flag = h264_u(buf, &bitpos, 1);
		printf("qp_prime_y_zero_transform_bypass_flag = %x\n",qp_prime_y_zero_transform_bypass_flag);

		int seq_scaling_matrix_present_flag = h264_u(buf, &bitpos, 1);
		printf("seq_scaling_matrix_present_flag = %x\n",seq_scaling_matrix_present_flag);
		int scalinglist4x4[16][16];
		int scalinglist8x8[16][64];
		int usedefaultscalingmatrix4x4flag[16];
		int usedefaultscalingmatrix8x8flag[16];
		if(seq_scaling_matrix_present_flag){
			u8 seq_scaling_list_present_flag[16];
			for(j=0;j<((thesps->chroma_format_idc != 3)?8:12);j++){
				seq_scaling_list_present_flag[j] = h264_u(buf, &bitpos, 1);
				if(j < 6){
					bitpos = scaling_list(buf, bitpos,
						scalinglist4x4[j], 16, usedefaultscalingmatrix4x4flag[j]);
				}
				else{
					bitpos = scaling_list(buf, bitpos,
						scalinglist8x8[j-6], 64, usedefaultscalingmatrix8x8flag[j-6]);
				}
			}
		}
	}

	thesps->log2_max_frame_num = h264_ue(buf, &bitpos) + 4;
	printf("log2_max_frame_num = %x\n", thesps->log2_max_frame_num);

	thesps->pic_order_cnt_type = h264_ue(buf, &bitpos);
	printf("pic_order_cnt_type = %x\n", thesps->pic_order_cnt_type);
	if(0 == thesps->pic_order_cnt_type){
		thesps->log2_max_pic_order_cnt_lsb = h264_ue(buf, &bitpos)+4;
		printf("log2_max_pic_order_cnt_lsb = %x\n",thesps->log2_max_pic_order_cnt_lsb);
	}
	else if(1 == thesps->pic_order_cnt_type){
		thesps->delta_pic_order_always_zero_flag = h264_u(buf, &bitpos, 1);
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

	thesps->frame_mbs_only_flag = h264_u(buf, &bitpos, 1);
	printf("frame_mbs_only_flag = %x\n", thesps->frame_mbs_only_flag);
	if(!thesps->frame_mbs_only_flag){
		thesps->mb_adaptive_frame_field_flag = h264_u(buf, &bitpos, 1);
		printf("mb_adaptive_frame_field_flag = %x\n", thesps->mb_adaptive_frame_field_flag);
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
		bitpos = parseh264_sps_vui(buf, bitpos);
	}
printf("}sps(%x,%x)\n\n",bitpos,len<<3);
	return 0;
}




/*
pic_parameter_set_rbsp( ) {
	pic_parameter_set_id    ue(v)
	seq_parameter_set_id    ue(v)
	entropy_coding_mode_flag   u(1)
	bottom_field_pic_order_in_frame_present_flag   u(1)
	num_slice_groups_minus1   ue(v)
	if( num_slice_groups_minus1 > 0 ) {
		slice_group_map_type   ue(v)
		if( slice_group_map_type = = 0 )
		{
			for( iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++ )
			{
				run_length_minus1[ iGroup ]   ue(v)
			}
		}
		else if( slice_group_map_type = = 2 )
		{
			for( iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++ )
			{
				top_left[ iGroup ]    ue(v)
				bottom_right[ iGroup ]    ue(v)
			}
		}
		else if( slice_group_map_type = = 3 | 
			slice_group_map_type = = 4 |
			slice_group_map_type = = 5 )
		{
			slice_group_change_direction_flag   u(1)
			slice_group_change_rate_minus1   ue(v)
		}
		else if( slice_group_map_type = = 6 )
		{
			pic_size_in_map_units_minus1    ue(v)
			for( i = 0; i <= pic_size_in_map_units_minus1; i++ )
				slice_group_id[ i ]    u(v)
			}
		}
	}

	num_ref_idx_l0_default_active_minus1  ue(v)
	num_ref_idx_l1_default_active_minus1  ue(v)
	weighted_pred_flag    u(1)
	weighted_bipred_idc   u(2)
	pic_init_qp_minus26   se(v)
	pic_init_qs_minus26   se(v)
	chroma_qp_index_offset   se(v)
	deblocking_filter_control_present_flag   u(1)
	constrained_intra_pred_flag   u(1)
	redundant_pic_cnt_present_flag   u(1)

	if( more_rbsp_data( ) ) {
		transform_8x8_mode_flag   u(1)
		pic_scaling_matrix_present_flag   u(1)
		if( pic_scaling_matrix_present_flag )
		{
			for( i = 0; i < 6 +( ( chroma_format_idc != 3 ) ? 2 : 6 ) * transform_8x8_mode_flag;i++ )
			{
				pic_scaling_list_present_flag[ i ]   u(1)
				if( pic_scaling_list_present_flag[ i ] ){
					if( i < 6 )
					{
						scaling_list( ScalingList4x4[ i ], 16,  UseDefaultScalingMatrix4x4Flag[ i ] )
					}
					else
					{
						scaling_list( ScalingList8x8[ i − 6 ], 64, UseDefaultScalingMatrix8x8Flag[ i − 6 ] )
					}
				}
			}
		}
		second_chroma_qp_index_offset   se(v)
	}
	rbsp_trailing_bits( )
}*/
struct pps_parsed{
	u32 pic_parameter_set_id;
	u32 seq_parameter_set_id;
	u32 entropy_coding_mode_flag;
	u32 bottom_field_pic_order_in_frame_present_flag;
	u32 num_slice_groups_minus1;
	u32 slice_group_map_type;
	u32 run_length_minus1[8];
	u32 top_left[8];
	u32 bottom_right[8];
	u32 slice_group_change_direction_flag;
	u32 slice_group_change_rate_minus1;
	u32 pic_size_in_map_units_minus1;
	u32 slice_group_id[8];
	u32 num_ref_idx_l0_default_active_minus1;
	u32 num_ref_idx_l1_default_active_minus1;
	u32 weighted_pred_flag;
	u32 weighted_bipred_idc;
	u32 pic_init_qp_minus26;
	u32 pic_init_qs_minus26;
	u32 chroma_qp_index_offset;
	u32 deblocking_filter_control_present_flag;
	u32 constrained_intra_pred_flag;
	u32 redundant_pic_cnt_present_flag;
	u32 transform_8x8_mode_flag;
	u32 pic_scaling_matrix_present_flag;
	u32 pic_scaling_list_present_flag[16];
	u32 ScalingList4x4[16][16];
	u32 UseDefaultScalingMatrix4x4Flag[16];
	u32 ScalingList8x8[16][64];
	u32 UseDefaultScalingMatrix8x8Flag[16];
	u32 second_chroma_qp_index_offset;
};
static struct pps_parsed perpps[16]={};
static int pps_status[16]={};
void* setpps(int id)
{
	return &perpps[id];
}
void* getpps(int id)
{
	return &perpps[id];
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

	struct pps_parsed* thepps = setpps(pic_parameter_set_id);
	if(0 == thepps){
		printf("error:cannot setpps\n");
		return 0;
	}
	thepps->seq_parameter_set_id = seq_parameter_set_id;
	thepps->pic_parameter_set_id = pic_parameter_set_id;

	struct sps_parsed* thesps = getsps(seq_parameter_set_id);
	if(0 == thesps){
		printf("error:sps not found\n");
		return 0;
	}

	thepps->entropy_coding_mode_flag = h264_u(buf, &bitpos, 1);
	printf("entropy_coding_mode_flag = %x\n", thepps->entropy_coding_mode_flag);

	thepps->bottom_field_pic_order_in_frame_present_flag = h264_u(buf, &bitpos, 1);
	printf("bottom_field_pic_order_in_frame_present_flag = %x\n",thepps->bottom_field_pic_order_in_frame_present_flag);
	thepps->num_slice_groups_minus1 = h264_ue(buf, &bitpos);
	printf("num_slice_groups_minus1 = %x\n", thepps->num_slice_groups_minus1);
	if(thepps->num_slice_groups_minus1 > 0){
		thepps->slice_group_map_type = h264_ue(buf, &bitpos);
		printf("slice_group_map_type = %x\n", thepps->slice_group_map_type);

		switch(thepps->slice_group_map_type){
		case 0:
			for(j=0;j<thepps->num_slice_groups_minus1;j++){
				printf("%d:run_length_minus1 = %x\n", j, h264_ue(buf, &bitpos));
			}
			break;
		case 2:
			for(j=0;j<thepps->num_slice_groups_minus1;j++){
				printf("%d:top_left = %x\n", j, h264_ue(buf, &bitpos));
				printf("%d:run_length_minus1 = %x\n", j, h264_ue(buf, &bitpos));
			}
			break;
		case 3:
		case 4:
		case 5:
			printf("slice_group_change_direction_flag = %x\n", h264_u(buf, &bitpos, 1));
			thepps->slice_group_change_rate_minus1 = h264_ue(buf, &bitpos);
			printf("slice_group_change_rate_minus1 = %x\n", thepps->slice_group_change_rate_minus1);
			break;
		case 6:
			thepps->pic_size_in_map_units_minus1 = h264_ue(buf, &bitpos);
			printf("pic_size_in_map_units_minus1 = %x\n", thepps->pic_size_in_map_units_minus1);
			for(j=0;j<thepps->pic_size_in_map_units_minus1;j++){
				printf("%d:slice_group_id = %x\n", j, h264_ue(buf, &bitpos));	//todo: it is u(v)
			}
			break;
		}
	}

	int num_ref_idx_10_default_active_minus1 = h264_ue(buf, &bitpos);
	printf("num_ref_idx_10_default_active_minus1 = %x\n", num_ref_idx_10_default_active_minus1);
	int num_ref_idx_11_default_active_minus1 = h264_ue(buf, &bitpos);
	printf("num_ref_idx_11_default_active_minus1 = %x\n", num_ref_idx_11_default_active_minus1);

	thepps->weighted_pred_flag = h264_u(buf, &bitpos, 1);
	printf("weighted_pred_flag = %x\n", thepps->weighted_pred_flag);
	thepps->weighted_bipred_idc = h264_u(buf, &bitpos, 2);
	printf("weighted_bipred_idc = %x\n", thepps->weighted_bipred_idc);

	int pic_init_qp_minus26 = h264_se(buf, &bitpos);
	printf("pic_init_qp_minus26 = %x\n", pic_init_qp_minus26);
	int pic_init_qs_minus26 = h264_se(buf, &bitpos);
	printf("pic_init_qs_minus26 = %x\n", pic_init_qs_minus26);

	int chroma_qp_index_offset = h264_se(buf, &bitpos);
	printf("chroma_qp_index_offset = %x\n", chroma_qp_index_offset);

	thepps->deblocking_filter_control_present_flag = h264_u(buf, &bitpos, 1);
	printf("deblocking_filter_control_present_flag = %x\n", thepps->deblocking_filter_control_present_flag);
	int constrained_intra_pred_flag = h264_u(buf, &bitpos, 1);
	printf("constrained_intra_pred_flag = %x\n", constrained_intra_pred_flag);
	thepps->redundant_pic_cnt_present_flag = h264_u(buf, &bitpos, 1);
	printf("redundant_pic_cnt_present_flag = %x\n", thepps->redundant_pic_cnt_present_flag);

	if(bitpos >= (len+2)*8)goto done;

	int transform_8x8_mode_flag = h264_u(buf, &bitpos, 1);
	printf("transform_8x8_mode_flag = %x\n", transform_8x8_mode_flag);

	int pic_scaling_matrix_present_flag = h264_u(buf, &bitpos, 1);
	printf("pic_scaling_matrix_present_flag = %x\n", pic_scaling_matrix_present_flag);
	if(pic_scaling_matrix_present_flag){
		int xxxx = 6+transform_8x8_mode_flag*((thesps->chroma_format_idc!=3)?2:6);
		int pic_scaling_list_present_flag;
		int scalinglist4x4[16][16];
		int scalinglist8x8[16][64];
		int usedefaultscalingmatrix4x4flag[16];
		int usedefaultscalingmatrix8x8flag[16];
		for(j=0;j<xxxx;j++){
			pic_scaling_list_present_flag = h264_u(buf, &bitpos, 1);
			printf("pic_scaling_list_present_flag[%d] = %x\n", j, pic_scaling_list_present_flag);
			if(pic_scaling_list_present_flag){
				if(j<6){
					bitpos = scaling_list(buf, bitpos,
						scalinglist4x4[j], 16, usedefaultscalingmatrix4x4flag[j]);
				}
				else{
					bitpos = scaling_list(buf, bitpos,
						scalinglist8x8[j-6], 64, usedefaultscalingmatrix8x8flag[j-6]);
				}
			}
		}
	}

done:
printf("}pps(%x,%x)\n\n",bitpos,len<<3);
	return 0;
}




/*
pred_weight_table( ) {
	luma_log2_weight_denom	ue(v)
	if( ChromaArrayType != 0 ){
		chroma_log2_weight_denom	ue(v)
	}
	for( i = 0; i <= num_ref_idx_l0_active_minus1; i++ ) {
		luma_weight_l0_flag	u(1)
		if( luma_weight_l0_flag ) {
			luma_weight_l0[ i ]	se(v)
			luma_offset_l0[ i ]	se(v)
		}
		if( ChromaArrayType != 0 ) {
			chroma_weight_l0_flag	u(1)
			if( chroma_weight_l0_flag ){
			for( j =0; j < 2; j++ ) {
				chroma_weight_l0[ i ][ j ]	se(v)
				chroma_offset_l0[ i ][ j ]	se(v)
			}
			}
		}
	}

	if( slice_type % 5 = = 1 ){
		for( i = 0; i <= num_ref_idx_l1_active_minus1; i++ ) {
			luma_weight_l1_flag	u(1)
			if( luma_weight_l1_flag ) {
				luma_weight_l1[ i ]	se(v)
					luma_offset_l1[ i ]	se(v)
			}

			if( ChromaArrayType != 0 ) {
				chroma_weight_l1_flag	u(1)
				if( chroma_weight_l1_flag ){
				for( j = 0; j < 2; j++ ) {
					chroma_weight_l1[ i ][ j ]	se(v)
					chroma_offset_l1[ i ][ j ]	se(v)
				}
				}
			}
		}
	}
}*/
int pred_weight_table(u8* buf, int bitpos,
	int ChromaArrayType, int slicetype,
	int num_ref_idx_l0_active_minus1, int num_ref_idx_l1_active_minus1)
{
	int luma_log2_weight_denom = h264_ue(buf, &bitpos);
	if( ChromaArrayType != 0 ){
		int chroma_log2_weight_denom = h264_ue(buf, &bitpos);
	}
	int i,j;
	for( i = 0; i <= num_ref_idx_l0_active_minus1; i++ ) {
		u8 luma_weight_l0_flag = h264_u(buf, &bitpos, 1);
		if( luma_weight_l0_flag ) {
			//luma_weight_l0[ i ] = h264_se(buf, &bitpos);
			//luma_offset_l0[ i ] = h264_se(buf, &bitpos);
			h264_se(buf, &bitpos);
			h264_se(buf, &bitpos);
		}
		if( ChromaArrayType != 0 ) {
			u8 chroma_weight_l0_flag = h264_u(buf, &bitpos, 1);
			if( chroma_weight_l0_flag ){
			for( j =0; j < 2; j++ ) {
				//chroma_weight_l0[ i ][ j ] = h264_se(buf, &bitpos);
				//chroma_offset_l0[ i ][ j ] = h264_se(buf, &bitpos);
				h264_se(buf, &bitpos);
				h264_se(buf, &bitpos);
			}
			}
		}
	}

	if( slicetype == 1 ){
		for( i = 0; i <= num_ref_idx_l1_active_minus1; i++ ) {
			u8 luma_weight_l1_flag = h264_u(buf, &bitpos, 1);
			if( luma_weight_l1_flag ) {
				//luma_weight_l1[ i ] = h264_se(buf, &bitpos);
				//luma_offset_l1[ i ] = h264_se(buf, &bitpos);
				h264_se(buf, &bitpos);
				h264_se(buf, &bitpos);
			}

			if( ChromaArrayType != 0 ) {
				u8 chroma_weight_l1_flag = h264_u(buf, &bitpos, 1);
				if( chroma_weight_l1_flag ){
				for( j = 0; j < 2; j++ ) {
					//chroma_weight_l1[ i ][ j ] = h264_se(buf, &bitpos);
					//chroma_offset_l1[ i ][ j ] = h264_se(buf, &bitpos);
					h264_se(buf, &bitpos);
					h264_se(buf, &bitpos);
				}
				}
			}
		}
	}
	return bitpos;
}


/*
ref_pic_list_modification( ) {
	if(slice_type%5 != 2 && slice_type%5 != 4){
		ref_pic_list_modification_flag_l0	u(1)
		if( ref_pic_list_modification_flag_l0 ){
		do {
			modification_of_pic_nums_idc	ue(v)
			if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 ){
				abs_diff_pic_num_minus1	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 2 ){
				long_term_pic_num	ue(v)
			}
		} while( modification_of_pic_nums_idc != 3 )
		}
	}

	if(slice_type%5 == 1){
		ref_pic_list_modification_flag_l1	u(1)
		if( ref_pic_list_modification_flag_l1 ){
		do {
			modification_of_pic_nums_idc	ue(v)
			if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 ){
				abs_diff_pic_num_minus1	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 2 ){
				long_term_pic_num	ue(v)
			}
		} while( modification_of_pic_nums_idc != 3 )
		}
	}
}
ref_pic_list_mvc_modification( ) {
	if(slice_type%5 != 2 && slice_type%5 != 4){
		ref_pic_list_modification_flag_l0	u(1)
		if( ref_pic_list_modification_flag_l0 ){
		do {
			modification_of_pic_nums_idc	ue(v)
			if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 ){
				abs_diff_pic_num_minus1	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 2 ){
				long_term_pic_num	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 4 | | modification_of_pic_nums_idc = = 5 ){
				abs_diff_view_idx_minus1	ue(v)
			}
		} while( modification_of_pic_nums_idc != 3 )
		}
	}
	if(slice_type%5 == 1){
		ref_pic_list_modification_flag_l1	u(1)
		if( ref_pic_list_modification_flag_l1 ){
		do {
			modification_of_pic_nums_idc	ue(v)
			if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 ){
				abs_diff_pic_num_minus1	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 2 ){
				long_term_pic_num	ue(v)
			}
			else if( modification_of_pic_nums_idc = = 4 | | modification_of_pic_nums_idc = = 5 ){
				abs_diff_view_idx_minus1	ue(v)
			}
		} while( modification_of_pic_nums_idc != 3 )
		}
	}
}
*/
int ref_pic_list_modification(u8* buf, int bitpos, int slicetype) {
	if( (slicetype != 2) && (slicetype != 4) ) {
		u8 ref_pic_list_modification_flag_l0 = h264_u(buf, &bitpos, 1);
		u8 modification_of_pic_nums_idc;
		if( ref_pic_list_modification_flag_l0 ){
		do {
			modification_of_pic_nums_idc = h264_ue(buf, &bitpos);
			if( modification_of_pic_nums_idc == 0 | modification_of_pic_nums_idc == 1 ){
				int abs_diff_pic_num = h264_ue(buf, &bitpos)+1;
				printf("abs_diff_pic_num=%d\n",abs_diff_pic_num);
			}
			else if( modification_of_pic_nums_idc == 2 ){
				int long_term_pic_num = h264_ue(buf, &bitpos);
				printf("long_term_pic_num=%d\n",long_term_pic_num);
			}
		} while( modification_of_pic_nums_idc != 3 );
		}
	}
	if( slicetype == 1 ) {
		u8 ref_pic_list_modification_flag_l1 = h264_u(buf, &bitpos, 1);
		u8 modification_of_pic_nums_idc;
		if( ref_pic_list_modification_flag_l1 ){
		do {
			modification_of_pic_nums_idc = h264_ue(buf, &bitpos);
			if( modification_of_pic_nums_idc == 0 | modification_of_pic_nums_idc == 1 ){
				int abs_diff_pic_num = h264_ue(buf, &bitpos)+1;
				printf("abs_diff_pic_num=%d\n",abs_diff_pic_num);
			}
			else if( modification_of_pic_nums_idc == 2 ){
				int long_term_pic_num = h264_ue(buf, &bitpos);
				printf("long_term_pic_num=%d\n",long_term_pic_num);
			}
		} while( modification_of_pic_nums_idc != 3 );
		}
	}
	return bitpos;
}
int ref_pic_list_mvc_modification(u8* buf, int bitpos, int slicetype) {
	if( (slicetype != 2) && (slicetype != 4) ) {
		u8 ref_pic_list_modification_flag_l0 = h264_u(buf, &bitpos, 1);
		u8 modification_of_pic_nums_idc;
		if( ref_pic_list_modification_flag_l0 ){
		do {
			modification_of_pic_nums_idc = h264_ue(buf, &bitpos);
			if( modification_of_pic_nums_idc == 0 | modification_of_pic_nums_idc == 1 ){
				int abs_diff_pic_num = h264_ue(buf, &bitpos)+1;
				printf("abs_diff_pic_num=%d\n",abs_diff_pic_num);
			}
			else if( modification_of_pic_nums_idc == 2 ){
				int long_term_pic_num = h264_ue(buf, &bitpos);
				printf("long_term_pic_num=%d\n",long_term_pic_num);
			}
			else if( modification_of_pic_nums_idc == 4 | modification_of_pic_nums_idc == 5 ){
				int abs_diff_view_idx_minus1 = h264_ue(buf, &bitpos);
				printf("abs_diff_view_idx_minus1=%d\n",abs_diff_view_idx_minus1);
			}
		} while( modification_of_pic_nums_idc != 3 );
		}
	}
	if( slicetype == 1 ) {
		u8 ref_pic_list_modification_flag_l1 = h264_u(buf, &bitpos, 1);
		u8 modification_of_pic_nums_idc;
		if( ref_pic_list_modification_flag_l1 ){
		do {
			modification_of_pic_nums_idc = h264_ue(buf, &bitpos);
			if( modification_of_pic_nums_idc == 0 | modification_of_pic_nums_idc == 1 ){
				int abs_diff_pic_num = h264_ue(buf, &bitpos)+1;
				printf("abs_diff_pic_num=%d\n",abs_diff_pic_num);
			}
			else if( modification_of_pic_nums_idc == 2 ){
				int long_term_pic_num = h264_ue(buf, &bitpos);
				printf("long_term_pic_num=%d\n",long_term_pic_num);
			}
			else if( modification_of_pic_nums_idc == 4 | modification_of_pic_nums_idc == 5 ){
				int abs_diff_view_idx_minus1 = h264_ue(buf, &bitpos);
				printf("abs_diff_view_idx_minus1=%d\n",abs_diff_view_idx_minus1);
			}
		} while( modification_of_pic_nums_idc != 3 );
		}
	}
	return bitpos;
}

/*
dec_ref_pic_marking( ) {
	if( IdrPicFlag ) {
		no_output_of_prior_pics_flag	u(1)
		long_term_reference_flag	u(1)
	} else {
		adaptive_ref_pic_marking_mode_flag	u(1)
		if( adaptive_ref_pic_marking_mode_flag ){
		do {
			memory_management_control_operation	ue(v)
			if( memory_management_control_operation = = 1 | | memory_management_control_operation = = 3 ){
				difference_of_pic_nums_minus1	ue(v)
			}
			if(memory_management_control_operation = = 2 ){
				long_term_pic_num	ue(v)
			}
			if( memory_management_control_operation = = 3 | | memory_management_control_operation = = 6 ){
				long_term_frame_idx	ue(v)
			}
			if( memory_management_control_operation = = 4 ){
				max_long_term_frame_idx_plus1	ue(v)
			}
		} while( memory_management_control_operation != 0 )
		}
	}
}
*/
int dec_ref_pic_marking(u8* buf, int bitpos, int IdrPicFlag) {
	if( IdrPicFlag ) {
		u8 no_output_of_prior_pics_flag = h264_u(buf, &bitpos, 1);
		u8 long_term_reference_flag = h264_u(buf, &bitpos, 1);
	} else {
		u8 adaptive_ref_pic_marking_mode_flag = h264_u(buf, &bitpos, 1);
		if( adaptive_ref_pic_marking_mode_flag ){
		int memory_management_control_operation;
		do {
			memory_management_control_operation = h264_ue(buf, &bitpos);
			if( memory_management_control_operation == 1 | memory_management_control_operation == 3 ){
				int difference_of_pic_nums_minus1 = h264_ue(buf, &bitpos);
				printf("difference_of_pic_nums_minus1=%d\n",difference_of_pic_nums_minus1);
			}
			if(memory_management_control_operation == 2 ){
				int long_term_pic_num = h264_ue(buf, &bitpos);
				printf("long_term_pic_num=%d\n",long_term_pic_num);
			}
			if( memory_management_control_operation == 3 | memory_management_control_operation == 6 ){
				int long_term_frame_idx = h264_ue(buf, &bitpos);
				printf("long_term_frame_idx=%d\n",long_term_frame_idx);
			}
			if( memory_management_control_operation == 4 ){
				int max_long_term_frame_idx_plus1 = h264_ue(buf, &bitpos);
				printf("max_long_term_frame_idx_plus1=%d\n",max_long_term_frame_idx_plus1);
			}
		} while( memory_management_control_operation != 0 );
		}
	}
	return bitpos;
}

int more_rbsp_data()
{
	return 1;
}
int NextMbAddress(int n)
{
/*
	if(num_slice_groups_minus1==0){
		for(i=0;i<PicSizeInMapUnits;i++)mapUnitToSliceGroupMap[ i ] = 0;
	}
	else{
		switch(slice_group_map_type){
		case 0:		//8.2.2.1
			break;
		case 1:		//8.2.2.2
			break;
		case 2:		//8.2.2.3
			break;
		case 3:		//8.2.2.4
			break;
		case 4:		//8.2.2.5
			break;
		case 5:		//8.2.2.6
			break;
		case 6:		//8.2.2.7
			break;
		}
	}

int i;
for(i=0;i<PicSizeInMbs;i++){
	if(frame_mbs_only_flag==1)|(field_pic_flag==1){
		MbToSliceGroupMap[ i ] = mapUnitToSliceGroupMap[ i ];
	}
	else if(MbaffFrameFlag==1){
		MbToSliceGroupMap[ i ] = mapUnitToSliceGroupMap[ i / 2 ];
	}
	else if(frame_mbs_only_flag==0)&&(mb_adaptive_frame_field_flag==0)&&(field_pic_flag==0)){
		MbToSliceGroupMap[ i ] = mapUnitToSliceGroupMap[
			( i / ( 2 * PicWidthInMbs ) ) * PicWidthInMbs
			+ ( i % PicWidthInMbs ) ];
	}

	int i=n+1;
	int PicWidthInMbs = pic_width_in_mbs_minus1 + 1;
	int PicHeightInMbs = FrameHeightInMbs / ( 1 + field_pic_flag );
	int PicSizeInMbs = PicWidthInMbs = pic_width_in_mbs_minus1 + 1 * PicHeightInMbs;
	while( i < PicSizeInMbs && MbToSliceGroupMap[ i ] != MbToSliceGroupMap[ n ] )i++;
	nextMbAddress = i;
*/
	return 0;
}

/*
slice_data( ) {
	if( entropy_coding_mode_flag )
	while( !byte_aligned( ) )
		cabac_alignment_one_bit	f(1)

	CurrMbAddr = first_mb_in_slice * ( 1 + MbaffFrameFlag )
	moreDataFlag = 1
	prevMbSkipped = 0

	do {
		if( slice_type != I && slice_type != SI ){
			if( !entropy_coding_mode_flag ) {
				mb_skip_run		=ue(v)
				prevMbSkipped = ( mb_skip_run > 0 )
				for( i=0; i<mb_skip_run; i++ ){
					CurrMbAddr = NextMbAddress( CurrMbAddr )
				}
				if( mb_skip_run > 0 ){
					moreDataFlag = more_rbsp_data( )
				}
			} else {
				mb_skip_flag		=ae(v)
				moreDataFlag = !mb_skip_flag
			}
		}
		if( moreDataFlag ) {
			if( MbaffFrameFlag && ( CurrMbAddr % 2 = = 0 | |( CurrMbAddr % 2 = = 1 && prevMbSkipped ) ) ){
				mb_field_decoding_flag		=u(1)|ae(v)
			}
			macroblock_layer( ) 2 | 3
		}
		if( !entropy_coding_mode_flag ){
			moreDataFlag = more_rbsp_data( )
		}
		else {
			if( slice_type != I && slice_type != SI ){
				prevMbSkipped = mb_skip_flag
			}
			if( MbaffFrameFlag && CurrMbAddr % 2 = = 0 ){
				moreDataFlag = 1
			}
			else {
				end_of_slice_flag		=ae(v)
				moreDataFlag = !end_of_slice_flag
			}
		}
		CurrMbAddr = NextMbAddress( CurrMbAddr )
	} while( moreDataFlag )
}
*/
struct head_parsed{
	int field_pic_flag;
	int first_mb_in_slice;
};
struct head_parsed shead[] = {};
int slice_data(u8* buf, int bitpos,
	struct sps_parsed* thesps, struct pps_parsed* thepps,
	int slicetype)
{
	if( thepps->entropy_coding_mode_flag ){
		int t=bitpos/8;
		if(bitpos&7)printf("padding=...%x | %x | %x,%x...\n",buf[t-1],buf[t],buf[t+1],buf[t+2]);
		else printf("padding=...%x,%x | %x,%x...\n",buf[t-2],buf[t-1],buf[t+0],buf[t+1]);
		if(bitpos&7)bitpos += 8-(bitpos&7);
	}

	int MbaffFrameFlag = ( thesps->mb_adaptive_frame_field_flag && !shead->field_pic_flag );
	int CurrMbAddr = shead->first_mb_in_slice * ( 1 + MbaffFrameFlag );
	int moreDataFlag = 1;
	int prevMbSkipped = 0;
	//do {
		int i;
		int mb_skip_run;
		int mb_skip_flag;
		if( slicetype != _I && slicetype != _SI ){
			if( !thepps->entropy_coding_mode_flag ) {
				mb_skip_run = h264_ue(buf, &bitpos);
				printf("mb_skip_run=%x\n",mb_skip_run);
				prevMbSkipped = ( mb_skip_run > 0 );
				printf("prevMbSkipped=%x\n",prevMbSkipped);
				for( i=0; i<mb_skip_run; i++ ){
					CurrMbAddr = NextMbAddress( CurrMbAddr );
				}
				if( mb_skip_run > 0 ){
					moreDataFlag = more_rbsp_data( );
				}
			} else {
				//mb_skip_flag = h264_ae(buf, &bitpos);
				//moreDataFlag = !mb_skip_flag;
			}
		}
/*		if( moreDataFlag ) {
			if( MbaffFrameFlag && ( CurrMbAddr % 2 = = 0 | |( CurrMbAddr % 2 = = 1 && prevMbSkipped ) ) ){
				mb_field_decoding_flag		=u(1)|ae(v)
			}
			macroblock_layer( ) 2 | 3
		}
		if( !entropy_coding_mode_flag ){
			moreDataFlag = more_rbsp_data( )
		}
		else {
			if( slice_type != I && slice_type != SI ){
				prevMbSkipped = mb_skip_flag
			}
			if( MbaffFrameFlag && CurrMbAddr % 2 = = 0 ){
				moreDataFlag = 1
			}
			else {
				end_of_slice_flag		=ae(v)
				moreDataFlag = !end_of_slice_flag
			}
		}
		CurrMbAddr = NextMbAddress( CurrMbAddr )
*/
	//} while( moreDataFlag )

	return bitpos;
}


/*
slice_header( ) {
	first_mb_in_slice	ue(v)
	slice_type		ue(v)
	pic_parameter_set_id	ue(v)

	if( separate_colour_plane_flag = = 1 ){
		colour_plane_id		u(2)
	}

	frame_num	u(v)

	if( !frame_mbs_only_flag ) {
		field_pic_flag		u(1)
		if( field_pic_flag ){
			bottom_field_flag	u(1)
		}
	}

	IdrPicFlag = ( ( nal_unit_type = = 5 ) ? 1 : 0 )
	if( IdrPicFlag ){
		idr_pic_id		ue(v)
	}

	if( pic_order_cnt_type = = 0 ) {
		pic_order_cnt_lsb		u(v)
		if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag ){
			delta_pic_order_cnt_bottom	se(v)
		}
	}

	if( pic_order_cnt_type = = 1 && !delta_pic_order_always_zero_flag ) {
		delta_pic_order_cnt[ 0 ]	se(v)
		if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag )
			delta_pic_order_cnt[ 1 ]	se(v)
	}

	if( redundant_pic_cnt_present_flag ){
		redundant_pic_cnt	ue(v)
	}

	if( slice_type = = B ){
		direct_spatial_mv_pred_flag	u(1)
	}

	if( slice_type = = P | | slice_type = = SP | | slice_type = = B ) {
		num_ref_idx_active_override_flag	u(1)
		if( num_ref_idx_active_override_flag ) {
			num_ref_idx_l0_active_minus1	ue(v)
			if( slice_type = = B ){
				num_ref_idx_l1_active_minus1	ue(v)
			}
		}
	}

	if( nal_unit_type = = 20 | | nal_unit_type = = 21 ){
		ref_pic_list_mvc_modification( ) // specified in Annex H
	}
	else{
		ref_pic_list_modification( )
	}

	if(	( weighted_pred_flag && ( slice_type = = P | | slice_type = = SP ) ) | |
		( weighted_bipred_idc = = 1 && slice_type = = B ) ){
		pred_weight_table( )
	}
	if( nal_ref_idc != 0 ){
		dec_ref_pic_marking( )
	}
	if( entropy_coding_mode_flag && slice_type != I && slice_type != SI ){
		cabac_init_idc	ue(v)
	}
	slice_qp_delta	se(v)
	if( slice_type = = SP | | slice_type = = SI ) {
		if( slice_type = = SP ){
			sp_for_switch_flag	u(1)
		}
		slice_qs_delta	se(v)
	}

	if( deblocking_filter_control_present_flag ) {
		disable_deblocking_filter_idc	ue(v)
		if( disable_deblocking_filter_idc != 1 ) {
			slice_alpha_c0_offset_div2	se(v)
			slice_beta_offset_div2	se(v)
		}
	}

	if( num_slice_groups_minus1 > 0 && slice_group_map_type >= 3 && slice_group_map_type <= 5){
		slice_group_change_cycle	u(v)
	}
}

*/
void parseh264_slice(u8* buf, int len)
{
	int bitpos = 8;
	int nal_ref_idc = (buf[0]>>5)&3;
	int nal_unit_type = buf[0]&0x1f;
	switch(nal_unit_type){
	case 1:
		printf("slice{\n");
		break;
	case 5:
		printf("idr{\n");
		break;
	default:
		printf("slice?{\n");
		break;
	}

	shead->first_mb_in_slice = h264_ue(buf, &bitpos);
	printf("first_mb_in_slice=%x\n",shead->first_mb_in_slice);
	u32 slice_type = h264_ue(buf, &bitpos);
	printf("slice_type=%x(%s)\n",slice_type,h264_typetable[slice_type%5]);
	u32 pic_parameter_set_id = h264_ue(buf, &bitpos);
	printf("pic_parameter_set_id=%x\n",pic_parameter_set_id);

	struct pps_parsed* thepps = getpps(pic_parameter_set_id);
	if(0 == thepps){
		printf("error:pps not found\n");
		return;
	}
	struct sps_parsed* thesps = getsps(thepps->seq_parameter_set_id);
	if(0 == thepps){
		printf("error:sps not found\n");
		return;
	}

	if(thesps->separate_colour_plane_flag){
		u8 colour_plane_id = h264_u(buf,&bitpos,2);
		printf("colour_plane_id = %d\n",colour_plane_id);
	}

	u32 frame_num = h264_u(buf, &bitpos, thesps->log2_max_frame_num);
	printf("frame_num = %x\n",frame_num);

	shead->field_pic_flag = 0;
	if( !thesps->frame_mbs_only_flag ) {
		shead->field_pic_flag = h264_u(buf, &bitpos, 1);
		if( shead->field_pic_flag ){
			u8 bottom_field_flag = h264_u(buf, &bitpos, 1);
			printf("bottom_field_flag=%x\n",bottom_field_flag);
		}
	}

	u8 IdrPicFlag = ( nal_unit_type == 5 ) ? 1 : 0;
	if( IdrPicFlag ){
		int idr_pic_id = h264_ue(buf, &bitpos);
		printf("idr_pic_id=%x\n",idr_pic_id);
	}

	if( thesps->pic_order_cnt_type == 0 ) {
		int pic_order_cnt_lsb = h264_u(buf, &bitpos, thesps->log2_max_pic_order_cnt_lsb);
		printf("pic_order_cnt_lsb = %x\n",pic_order_cnt_lsb);
		if( thepps->bottom_field_pic_order_in_frame_present_flag && !shead->field_pic_flag ){
			int delta_pic_order_cnt_bottom = h264_se(buf, &bitpos);
			printf("delta_pic_order_cnt_bottom = %x\n",delta_pic_order_cnt_bottom);
		}
	}

	if( thesps->pic_order_cnt_type == 1 && !thesps->delta_pic_order_always_zero_flag ) {
		int delta_pic_order_cnt0 = h264_se(buf, &bitpos);
		printf("delta_pic_order_cnt[0]=%d\n",delta_pic_order_cnt0);
		if( thepps->bottom_field_pic_order_in_frame_present_flag && !shead->field_pic_flag ){
			int delta_pic_order_cnt1 = h264_se(buf, &bitpos);
			printf("delta_pic_order_cnt[1]=%d\n",delta_pic_order_cnt1);
		}
	}

	if( thepps->redundant_pic_cnt_present_flag ){
		int redundant_pic_cnt = h264_ue(buf, &bitpos);
		printf("redundant_pic_cnt=%d\n",redundant_pic_cnt);
	}

	int slicetype = slice_type%5;;
	if( slicetype == 1 ){	//B
		u8 direct_spatial_mv_pred_flag = h264_u(buf, &bitpos, 1);
		printf("direct_spatial_mv_pred_flag=%d\n",direct_spatial_mv_pred_flag);
	}
	int num_ref_idx_l0_active_minus1 = 0;
	int num_ref_idx_l1_active_minus1 = 0;
	if( (slicetype == 0) | (slicetype == 1) | (slicetype == 3) ){
		u8 num_ref_idx_active_override_flag = h264_u(buf, &bitpos, 1);
		if( num_ref_idx_active_override_flag ) {
			num_ref_idx_l0_active_minus1 = h264_ue(buf, &bitpos);
			printf("num_ref_idx_l0_active_minus1=%d\n",num_ref_idx_l0_active_minus1);
			if( slicetype == 1 ){
				num_ref_idx_l1_active_minus1 = h264_ue(buf, &bitpos);
				printf("num_ref_idx_l1_active_minus1=%d\n",num_ref_idx_l1_active_minus1);
			}
		}
	}

	if( (nal_unit_type == 20) | (nal_unit_type == 21) ){
		ref_pic_list_mvc_modification(buf, bitpos, slicetype);
	}
	else{
		bitpos = ref_pic_list_modification(buf, bitpos, slicetype);
	}

	if(	( thepps->weighted_pred_flag && ( slicetype == 0 | slicetype == 3 ) ) |
		( thepps->weighted_bipred_idc == 1 && slicetype == 1 ) ){
		int ChromaArrayType = thesps->separate_colour_plane_flag ? 0 : thesps->chroma_format_idc;
		bitpos = pred_weight_table(buf, bitpos,
				ChromaArrayType, slicetype,
				num_ref_idx_l0_active_minus1, num_ref_idx_l1_active_minus1);
	}
	if( nal_ref_idc != 0 ){
		bitpos = dec_ref_pic_marking(buf, bitpos, IdrPicFlag);
	}
	if( thepps->entropy_coding_mode_flag && slicetype != 2 && slicetype != 4 ){
		int cabac_init_idc = h264_ue(buf, &bitpos);
		printf("cabac_init_idc=%x\n",cabac_init_idc);
	}

	int slice_qp_delta = h264_se(buf, &bitpos);
	printf("slice_qp_delta=%d\n",slice_qp_delta);

	if( slicetype == 3 | slicetype == 4 ) {
		if( slicetype == 3 ){
			u8 sp_for_switch_flag = h264_u(buf, &bitpos, 1);
		}
		int slice_qs_delta = h264_se(buf, &bitpos);
	}

	if( thepps->deblocking_filter_control_present_flag ) {
		int disable_deblocking_filter_idc = h264_ue(buf, &bitpos);
		if( disable_deblocking_filter_idc != 1 ) {
			int slice_alpha_c0_offset_div2 = h264_se(buf, &bitpos);
			int slice_beta_offset_div2 = h264_se(buf, &bitpos);
		}
	}

	if( thepps->num_slice_groups_minus1 > 0 && thepps->slice_group_map_type >= 3 && thepps->slice_group_map_type <= 5){
		//int vv = Ceil( Log2( pic_size_in_map_units / slice_group_change_rate + 1 ) );
		int vv = h264_log2( (thepps->pic_size_in_map_units_minus1+1) / (thepps->slice_group_change_rate_minus1+1) + 1 );
		int slice_group_change_cycle = h264_u(buf, &bitpos, vv);
		printf("slice_group_change_cycle=%x\n",slice_group_change_cycle);
	}

	printf("----(%x,%x)\n",bitpos,len<<3);

	bitpos = slice_data(buf, bitpos,
		thesps, thepps,
		slicetype);

	printf("}slice(%x,%x)\n\n",bitpos,len<<3);
}




int parseh264(unsigned char* buf, int len)
{
	switch(buf[0]&0x1f){
	case 1:
		parseh264_slice(buf, len);
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
		parseh264_slice(buf, len);
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
