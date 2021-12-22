#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;




u16 parseh265_swap16(u16 in)
{
        return ((in<<8)&0xff00) | ((in>>8)&0xff);
}
u32 parseh265_swap32(u32 in)
{
        return (in>>24) | ((in>>8)&0xff00) | ((in<<8)&0xff0000) | (in<<24);
}
u64 parseh265_swap64(u64 in)
{
        u64 lo = in&0xffffffff;
        u64 hi = (in>>32)&0xffffffff;
        lo = parseh265_swap32(lo);
        hi = parseh265_swap32(hi);
        return (lo<<32) | hi;
}
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



/*
spect_ratio_idc Sample aspect		ratio		Examples of use (informative)
0	Unspecified
1	1:1("square")
		7680x4320 16:9 frame without horizontal overscan
		3840x2160 16:9 frame without horizontal overscan
		1280x720 16:9 frame without horizontal overscan
		1920x1080 16:9 frame without horizontal overscan (cropped from 1920x1088)
		640x480 4:3 frame without horizontal overscan
2	12:11
		720x576 4:3 frame with horizontal overscan
		352x288 4:3 frame without horizontal overscan
3	10:11
		720x480 4:3 frame with horizontal overscan
		352x240 4:3 frame without horizontal overscan
4	16:11
		720x576 16:9 frame with horizontal overscan
		528x576 4:3 frame without horizontal overscan
5	40:33
		720x480 16:9 frame with horizontal overscan
		528x480 4:3 frame without horizontal overscan
6	24:11
		352x576 4:3 frame without horizontal overscan
		480x576 16:9 frame with horizontal overscan
7	20:11
		352x480 4:3 frame without horizontal overscan
		480x480 16:9 frame with horizontal overscan
8	32:11
		352x576 16:9 frame without horizontal overscan
9	80:33
		352x480 16:9 frame without horizontal overscan
10	18:11
		480x576 4:3 frame with horizontal overscan
11	15:11
		480x480 4:3 frame with horizontal overscan
12	64:33
		528x576 16:9 frame without horizontal overscan
13	160:99
		528x480 16:9 frame without horizontal overscan
14	4:3
		1440x1080 16:9 frame without horizontal overscan
15	3:2
		1280x1080 16:9 frame without horizontal overscan
16	2:1
		960x1080 16:9 frame without horizontal overscan
255	EXTENDED_SAR
*/




int h265_u(unsigned char* buf, int* pos, int cnt)
{
        int j,aa,bb;
        int ret = 0;
//printf("@h265_u:bitpos=%x,cnt=%x\n", *pos, cnt);
        for(j=0;j<cnt;j++){
                ret = ret<<1;
                aa = (*pos)/8;
                bb = (*pos)%8;
                if((buf[aa] & (0x80>>bb)) != 0)ret |= 1;
                *pos += 1;
        }
        return ret;
}
int h265_ue(unsigned char* buf, int* pos)
{
        int aa,bb;
        int ret, cnt = 0;
//printf("@h265_ue+++:bitpos=%x,cnt=%d\n", *pos, cnt);
        while(1){
                aa = (*pos)/8;
                bb = (*pos)%8;
                if((buf[aa] & (0x80>>bb)) != 0)break;
                cnt += 1;
                *pos += 1;
        }
        ret = h265_u(buf, pos, cnt+1) - 1;

//printf("@h265_ue---:bitpos=%x,cnt=%d\n", *pos, cnt);
        return ret;
}
int h265_se(unsigned char* buf, int* pos)
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

	ret = h265_u(buf, pos, cnt);

	aa = (*pos)/8;
	bb = (*pos)%8;
	if((buf[aa] & (0x80>>bb)) != 0)ret = -ret;
	*pos += 1;

	return ret;
}




/*
scaling_list_data( ) {
	for( sizeId = 0; sizeId < 4; sizeId++ ){
		for( matrixId = 0; matrixId < 6; matrixId += ( sizeId = = 3 ) ? 3 : 1 ) {
			scaling_list_pred_mode_flag[ sizeId ][ matrixId ] u(1)
			if( !scaling_list_pred_mode_flag[ sizeId ][ matrixId ] )
				scaling_list_pred_matrix_id_delta[ sizeId ][ matrixId ] ue(v)
			else {
				nextCoef = 8
				coefNum = Min( 64, ( 1 << ( 4 + ( sizeId << 1 ) ) ) )
				if( sizeId > 1 ) {
					scaling_list_dc_coef_minus8[ sizeId − 2 ][ matrixId ] se(v)
					nextCoef = scaling_list_dc_coef_minus8[ sizeId − 2 ][ matrixId ] + 8
				}
				for( i = 0; i < coefNum; i++ ) {
					scaling_list_delta_coef se(v)
					nextCoef = ( nextCoef + scaling_list_delta_coef + 256 ) % 256
					ScalingList[ sizeId ][ matrixId ][ i ] = nextCoef
				}
			}
		}
	}
}
*/
int scaling_list_data(u8* buf, int bitpos) {
	int sizeId;
	int matrixId;
	u8 scaling_list_pred_mode_flag;
	int scaling_list_pred_matrix_id_delta[4][6];
	int scaling_list_dc_coef_minus8[4][6];
	int ScalingList[4][6][64];
        for( sizeId = 0; sizeId < 4; sizeId++ ){
                for( matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1 ) {
                        scaling_list_pred_mode_flag = h265_u(buf, &bitpos, 1);
                        if( !scaling_list_pred_mode_flag){
                                scaling_list_pred_matrix_id_delta[sizeId][matrixId]  = h265_ue(buf, &bitpos);
                        }
                        else {
                                int nextCoef = 8;
                                int coefNum = 1 << ( 4 + ( sizeId << 1 ) );
                                if(coefNum > 64)coefNum = 64;
                                if( sizeId > 1 ) {
                                        scaling_list_dc_coef_minus8[sizeId-2][matrixId] = h265_se(buf, &bitpos);
                                        nextCoef = scaling_list_dc_coef_minus8[sizeId-2][matrixId] + 8;
                                }

                                int i;
                                for( i = 0; i < coefNum; i++ ) {
                                        int scaling_list_delta_coef = h265_se(buf, &bitpos);
                                        int nextCoef = ( nextCoef + scaling_list_delta_coef + 256 ) % 256;
                                        ScalingList[sizeId][matrixId][i] = nextCoef;
                                }
                        }
                }
        }
        return bitpos;
}


/*
profile_tier_level( profilePresentFlag, maxNumSubLayersMinus1 ) {
	if( profilePresentFlag ) {
		general_profile_space u(2)
		general_tier_flag u(1)
		general_profile_idc u(5)
		for( j = 0; j < 32; j++ ){
			general_profile_compatibility_flag[ j ] u(1)
		}
		general_progressive_source_flag u(1)
		general_interlaced_source_flag u(1)
		general_non_packed_constraint_flag u(1)
		general_frame_only_constraint_flag u(1)
		if( general_profile_idc = = 4 | | general_profile_compatibility_flag[ 4 ] | |
		general_profile_idc = = 5 | | general_profile_compatibility_flag[ 5 ] | |
		general_profile_idc = = 6 | | general_profile_compatibility_flag[ 6 ] | |
		general_profile_idc = = 7 | | general_profile_compatibility_flag[ 7 ] | |
		general_profile_idc = = 8 | | general_profile_compatibility_flag[ 8 ] | |
		general_profile_idc = = 9 | | general_profile_compatibility_flag[ 9 ] | |
		general_profile_idc = = 10 | | general_profile_compatibility_flag[ 10 ] | |
		general_profile_idc = = 11 | | general_profile_compatibility_flag[ 11 ] )
		{
			general_max_12bit_constraint_flag u(1)
			general_max_10bit_constraint_flag u(1)
			general_max_8bit_constraint_flag u(1)
			general_max_422chroma_constraint_flag u(1)
			general_max_420chroma_constraint_flag u(1)
			general_max_monochrome_constraint_flag u(1)
			general_intra_constraint_flag u(1)
			general_one_picture_only_constraint_flag u(1)
			general_lower_bit_rate_constraint_flag u(1)
			if( general_profile_idc = = 5 | | general_profile_compatibility_flag[ 5 ] | |
			general_profile_idc = = 9 | | general_profile_compatibility_flag[ 9 ] | |
			general_profile_idc = = 10 | | general_profile_compatibility_flag[ 10 ] | |
			general_profile_idc = = 11 | | general_profile_compatibility_flag[ 11 ] )
			{
				general_max_14bit_constraint_flag u(1)
				general_reserved_zero_33bits u(33)
			} else{
				general_reserved_zero_34bits u(34)
			}
		} else if( general_profile_idc = = 2 | | general_profile_compatibility_flag[ 2 ] ) {
			general_reserved_zero_7bits u(7)
			general_one_picture_only_constraint_flag u(1)
			general_reserved_zero_35bits u(35)
		} else
			general_reserved_zero_43bits u(43)
		}

		if( general_profile_idc = = 1 | | general_profile_compatibility_flag[ 1 ] | |
		general_profile_idc = = 2 | | general_profile_compatibility_flag[ 2 ] | |
		general_profile_idc = = 3 | | general_profile_compatibility_flag[ 3 ] | |
		general_profile_idc = = 4 | | general_profile_compatibility_flag[ 4 ] | |
		general_profile_idc = = 5 | | general_profile_compatibility_flag[ 5 ] | |
		general_profile_idc = = 9 | | general_profile_compatibility_flag[ 9 ] | |
		general_profile_idc = = 11 | | general_profile_compatibility_flag[ 11 ] )
		{
			general_inbld_flag u(1)
		}
		else{
			general_reserved_zero_bit u(1)
		}
	}//if( profilePresentFlag ) 

	general_level_idc u(8)
	for( i = 0; i < maxNumSubLayersMinus1; i++ ) {
		sub_layer_profile_present_flag[ i ] u(1)
		sub_layer_level_present_flag[ i ] u(1)
	}

	if( maxNumSubLayersMinus1 > 0 ){
		for( i = maxNumSubLayersMinus1; i < 8; i++ ){
			reserved_zero_2bits[ i ] u(2)
		}
	}

	for( i = 0; i < maxNumSubLayersMinus1; i++ ) {
		if( sub_layer_profile_present_flag[ i ] ) {
			sub_layer_profile_space[ i ] u(2)
			sub_layer_tier_flag[ i ] u(1)
			sub_layer_profile_idc[ i ] u(5)
			for( j = 0; j < 32; j++ ){
				sub_layer_profile_compatibility_flag[ i ][ j ] u(1)
			}
			sub_layer_progressive_source_flag[ i ] u(1)
			sub_layer_interlaced_source_flag[ i ] u(1)
			sub_layer_non_packed_constraint_flag[ i ] u(1)
			sub_layer_frame_only_constraint_flag[ i ] u(1)
			if( sub_layer_profile_idc[ i ] = = 4 | |
			sub_layer_profile_compatibility_flag[ i ][ 4 ] | |
			sub_layer_profile_idc[ i ] = = 5 | |
			sub_layer_profile_compatibility_flag[ i ][ 5 ] | |
			sub_layer_profile_idc[ i ] = = 6 | |
			sub_layer_profile_compatibility_flag[ i ][ 6 ] | |
			sub_layer_profile_idc[ i ] = = 7 | |
			sub_layer_profile_compatibility_flag[ i ][ 7 ] | |
			sub_layer_profile_idc[ i ] = = 8 | |
			sub_layer_profile_compatibility_flag[ i ][ 8 ] | |
			sub_layer_profile_idc[ i ] = = 9 | |
			sub_layer_profile_compatibility_flag[ i ][ 9 ] | |
			sub_layer_profile_idc[ i ] = = 10 | |
			sub_layer_profile_compatibility_flag[ i ][ 10 ] | |
			sub_layer_profile_idc[ i ] = = 11 | |
			sub_layer_profile_compatibility_flag[ i ][ 11 ] )
			{
				sub_layer_max_12bit_constraint_flag[ i ] u(1)
				sub_layer_max_10bit_constraint_flag[ i ] u(1)
				sub_layer_max_8bit_constraint_flag[ i ] u(1)
				sub_layer_max_422chroma_constraint_flag[ i ] u(1)
				sub_layer_max_420chroma_constraint_flag[ i ] u(1)
				sub_layer_max_monochrome_constraint_flag[ i ] u(1)
				sub_layer_intra_constraint_flag[ i ] u(1)
				sub_layer_one_picture_only_constraint_flag[ i ] u(1)
				sub_layer_lower_bit_rate_constraint_flag[ i ] u(1)
				if( sub_layer_profile_idc[ i ] = = 5 | |
				sub_layer_profile_compatibility_flag[ i ][ 5 ] | |
				sub_layer_profile_idc[ i ] = = 9 | |
				sub_layer_profile_compatibility_flag[ i ][ 9 ] | |
				sub_layer_profile_idc[ i ] = = 10 | |
				sub_layer_profile_compatibility_flag[ i ][ 10 ] | |
				sub_layer_profile_idc[ i ] = = 11 | |
				sub_layer_profile_compatibility_flag[ i ][ 11 ] )
				{
					sub_layer_max_14bit_constraint_flag[ i ] u(1)
					sub_layer_reserved_zero_33bits[ i ] u(33)
				} else
					sub_layer_reserved_zero_34bits[ i ] u(34)
			} else if( sub_layer_profile_idc[ i ] = = 2 | | sub_layer_profile_compatibility_flag[ i ][ 2 ] ) {
				sub_layer_reserved_zero_7bits[ i ] u(7)
				sub_layer_one_picture_only_constraint_flag[ i ] u(1)
				sub_layer_reserved_zero_35bits[ i ] u(35)
			} else{
				sub_layer_reserved_zero_43bits[ i ] u(43)
			}

			if( sub_layer_profile_idc[ i ] = = 1 | |
				sub_layer_profile_compatibility_flag[ i ][ 1 ] | |
				sub_layer_profile_idc[ i ] = = 2 | |
				sub_layer_profile_compatibility_flag[ i ][ 2 ] | |
				sub_layer_profile_idc[ i ] = = 3 | |
				sub_layer_profile_compatibility_flag[ i ][ 3 ] | |
				sub_layer_profile_idc[ i ] = = 4 | |
				sub_layer_profile_compatibility_flag[ i ][ 4 ] | |
				sub_layer_profile_idc[ i ] = = 5 | |
				sub_layer_profile_compatibility_flag[ i ][ 5 ] | |
				sub_layer_profile_idc[ i ] = = 9 | |
				sub_layer_profile_compatibility_flag[ i ][ 9 ] | |
				sub_layer_profile_idc[ i ] = = 11 | |
				sub_layer_profile_compatibility_flag[ i ][ 11 ] )
			{
				sub_layer_inbld_flag[ i ] u(1)
			}
			else{
				sub_layer_reserved_zero_bit[ i ] u(1)
			}
		}//if( sub_layer_profile_present_flag[ i ] )

		if( sub_layer_level_present_flag[ i ] ){
			sub_layer_level_idc[ i ] u(8)
		}
	}//for( i = 0; i < maxNumSubLayersMinus1; i++ )
}*/
int profile_tier_level(u8* buf,int bitpos, int profilePresentFlag,int maxNumSubLayersMinus1)
{
printf("profile_tier_level{\n");
	int i,j;
	u8 general_profile_idc;
	u8 general_profile_compatibility_flag[32];
	if( profilePresentFlag ) {
		u8 general_profile_space = h265_u(buf,&bitpos,2);
		u8 general_tier_flag = h265_u(buf,&bitpos,1);
		general_profile_idc = h265_u(buf,&bitpos,5);
		for( j = 0; j < 32; j++ ){
			general_profile_compatibility_flag[ j ] = h265_u(buf,&bitpos,1);
		}
		u8 general_progressive_source_flag = h265_u(buf,&bitpos,1);
		u8 general_interlaced_source_flag = h265_u(buf,&bitpos,1);
		u8 general_non_packed_constraint_flag = h265_u(buf,&bitpos,1);
		u8 general_frame_only_constraint_flag = h265_u(buf,&bitpos,1);
		if(	(general_profile_idc == 4) | general_profile_compatibility_flag[ 4 ] |
			(general_profile_idc == 5) | general_profile_compatibility_flag[ 5 ] |
			(general_profile_idc == 6) | general_profile_compatibility_flag[ 6 ] |
			(general_profile_idc == 7) | general_profile_compatibility_flag[ 7 ] |
			(general_profile_idc == 8) | general_profile_compatibility_flag[ 8 ] |
			(general_profile_idc == 9) | general_profile_compatibility_flag[ 9 ] |
			(general_profile_idc ==10) | general_profile_compatibility_flag[ 10 ] |
			(general_profile_idc ==11) | general_profile_compatibility_flag[ 11 ] )
		{
			u8 general_max_12bit_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_max_10bit_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_max_8bit_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_max_422chroma_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_max_420chroma_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_max_monochrome_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_intra_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_one_picture_only_constraint_flag = h265_u(buf,&bitpos,1);
			u8 general_lower_bit_rate_constraint_flag = h265_u(buf,&bitpos,1);
			if(	(general_profile_idc == 5) | general_profile_compatibility_flag[ 5 ] |
				(general_profile_idc == 9) | general_profile_compatibility_flag[ 9 ] |
				(general_profile_idc ==10) | general_profile_compatibility_flag[ 10 ] |
				(general_profile_idc ==11) | general_profile_compatibility_flag[ 11 ] )
			{
				u8 general_max_14bit_constraint_flag = h265_u(buf,&bitpos,1);
				bitpos += 33;
			} else{
				bitpos += 34;
			}
		}
		else if((general_profile_idc == 2) | general_profile_compatibility_flag[ 2 ] ) {
			bitpos += 7;
			u8 general_one_picture_only_constraint_flag = h265_u(buf,&bitpos,1);
			bitpos += 35;
		}
		else{
			bitpos += 43;
		}

		if(	(general_profile_idc == 1) | general_profile_compatibility_flag[ 1 ] |
			(general_profile_idc == 2) | general_profile_compatibility_flag[ 2 ] |
			(general_profile_idc == 3) | general_profile_compatibility_flag[ 3 ] |
			(general_profile_idc == 4) | general_profile_compatibility_flag[ 4 ] |
			(general_profile_idc == 5) | general_profile_compatibility_flag[ 5 ] |
			(general_profile_idc == 9) | general_profile_compatibility_flag[ 9 ] |
			(general_profile_idc ==11) | general_profile_compatibility_flag[ 11 ] )
		{
			u8 general_inbld_flag = h265_u(buf,&bitpos,1);
		}
		else{
			bitpos += 1;
		}

	}//if( profilePresentFlag ) 

	u8 general_level_idc = h265_u(buf,&bitpos,8);
	printf("general_level_idc = %d\n",general_level_idc);
	u8 sub_layer_profile_present_flag[32];
	u8 sub_layer_level_present_flag[32];
	for( i = 0; i < maxNumSubLayersMinus1; i++ ) {
		sub_layer_profile_present_flag[ i ] = h265_u(buf,&bitpos,1);
		sub_layer_level_present_flag[ i ] = h265_u(buf,&bitpos,1);
	}

	if( maxNumSubLayersMinus1 > 0 ){
		for( i = maxNumSubLayersMinus1; i < 8; i++ ){
			bitpos += 2;
		}
	}

	u8 sub_layer_profile_space[32];
	u8 sub_layer_tier_flag[32];
	u8 sub_layer_profile_idc[32];
	u8 sub_layer_profile_compatibility_flag[32][32];
	u8 sub_layer_progressive_source_flag[32];
	u8 sub_layer_interlaced_source_flag[32];
	u8 sub_layer_non_packed_constraint_flag[32];
	u8 sub_layer_frame_only_constraint_flag[32];
	u8 sub_layer_max_12bit_constraint_flag[32];
	u8 sub_layer_max_10bit_constraint_flag[32];
	u8 sub_layer_max_8bit_constraint_flag[32];
	u8 sub_layer_max_422chroma_constraint_flag[32];
	u8 sub_layer_max_420chroma_constraint_flag[32];
	u8 sub_layer_max_monochrome_constraint_flag[32];
	u8 sub_layer_intra_constraint_flag[32];
	u8 sub_layer_one_picture_only_constraint_flag[32];
	u8 sub_layer_lower_bit_rate_constraint_flag[32];
	u8 sub_layer_max_14bit_constraint_flag[32];
	u8 sub_layer_inbld_flag[32];
	u8 sub_layer_level_idc[32];
	for( i = 0; i < maxNumSubLayersMinus1; i++ ) {
		if( sub_layer_profile_present_flag[ i ] ) {
			sub_layer_profile_space[ i ] = h265_u(buf,&bitpos,2);
			sub_layer_tier_flag[ i ] = h265_u(buf,&bitpos,1);
			sub_layer_profile_idc[ i ] = h265_u(buf,&bitpos,5);
			for( j = 0; j < 32; j++ ){
				sub_layer_profile_compatibility_flag[ i ][ j ] = h265_u(buf,&bitpos,1);
			}
			sub_layer_progressive_source_flag[ i ] = h265_u(buf,&bitpos,1);
			sub_layer_interlaced_source_flag[ i ] = h265_u(buf,&bitpos,1);
			sub_layer_non_packed_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
			sub_layer_frame_only_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
			if(	(sub_layer_profile_idc[ i ] == 4) | sub_layer_profile_compatibility_flag[ i ][ 4 ] |
				(sub_layer_profile_idc[ i ] == 5) | sub_layer_profile_compatibility_flag[ i ][ 5 ] |
				(sub_layer_profile_idc[ i ] == 6) | sub_layer_profile_compatibility_flag[ i ][ 6 ] |
				(sub_layer_profile_idc[ i ] == 7) | sub_layer_profile_compatibility_flag[ i ][ 7 ] |
				(sub_layer_profile_idc[ i ] == 8) | sub_layer_profile_compatibility_flag[ i ][ 8 ] |
				(sub_layer_profile_idc[ i ] == 9) | sub_layer_profile_compatibility_flag[ i ][ 9 ] |
				(sub_layer_profile_idc[ i ] ==10) | sub_layer_profile_compatibility_flag[ i ][ 10 ] |
				(sub_layer_profile_idc[ i ] ==11) | sub_layer_profile_compatibility_flag[ i ][ 11 ] )
			{
				sub_layer_max_12bit_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_max_10bit_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_max_8bit_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_max_422chroma_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_max_420chroma_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_max_monochrome_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_intra_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_one_picture_only_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				sub_layer_lower_bit_rate_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				if(	(sub_layer_profile_idc[ i ] == 5) | sub_layer_profile_compatibility_flag[ i ][ 5 ] |
					(sub_layer_profile_idc[ i ] == 9) | sub_layer_profile_compatibility_flag[ i ][ 9 ] |
					(sub_layer_profile_idc[ i ] ==10) | sub_layer_profile_compatibility_flag[ i ][ 10 ] |
					(sub_layer_profile_idc[ i ] ==11) | sub_layer_profile_compatibility_flag[ i ][ 11 ] )
				{
					sub_layer_max_14bit_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
					bitpos += 33;
				} else{
					bitpos += 34;
				}
			} else if((sub_layer_profile_idc[ i ] == 2) | sub_layer_profile_compatibility_flag[ i ][ 2 ] ) {
				bitpos += 7;
				sub_layer_one_picture_only_constraint_flag[ i ] = h265_u(buf,&bitpos,1);
				bitpos += 35;
			} else{
				bitpos += 43;
			}

			if(	(sub_layer_profile_idc[ i ] == 1) | sub_layer_profile_compatibility_flag[ i ][ 1 ] |
				(sub_layer_profile_idc[ i ] == 2) | sub_layer_profile_compatibility_flag[ i ][ 2 ] |
				(sub_layer_profile_idc[ i ] == 3) | sub_layer_profile_compatibility_flag[ i ][ 3 ] |
				(sub_layer_profile_idc[ i ] == 4) | sub_layer_profile_compatibility_flag[ i ][ 4 ] |
				(sub_layer_profile_idc[ i ] == 5) | sub_layer_profile_compatibility_flag[ i ][ 5 ] |
				(sub_layer_profile_idc[ i ] == 9) | sub_layer_profile_compatibility_flag[ i ][ 9 ] |
				(sub_layer_profile_idc[ i ] ==11) | sub_layer_profile_compatibility_flag[ i ][ 11 ] )
			{
				sub_layer_inbld_flag[ i ] = h265_u(buf,&bitpos,1);
			}
			else{
				bitpos += 1;
			}
		}//if( sub_layer_profile_present_flag[ i ] )

		if( sub_layer_level_present_flag[ i ] ){
			sub_layer_level_idc[ i ] = h265_u(buf,&bitpos,8);
		}
	}//for( i = 0; i < maxNumSubLayersMinus1; i++ )

printf("}profile_tier_level\n");
	return bitpos;
}



/*
sub_layer_hrd_parameters( subLayerId ) {
	for( i = 0; i < CpbCnt; i++ ) {
		bit_rate_value_minus1[ i ] ue(v)
		cpb_size_value_minus1[ i ] ue(v)
		if( sub_pic_hrd_params_present_flag ) {
			cpb_size_du_value_minus1[ i ] ue(v)
			bit_rate_du_value_minus1[ i ] ue(v)
		}
		cbr_flag[ i ] u(1)
	}
}
*/
int sub_layer_hrd_parameters(u8* buf, int bitpos,
	int subLayerId, int sub_pic_hrd_params_present_flag,
	u32* cpb_cnt_minus1)
{
	int i;
	u32 bit_rate_value_minus1[32];
	u32 cpb_size_value_minus1[32];
	u32 cpb_size_du_value_minus1[32];
	u32 bit_rate_du_value_minus1[32];
	u8 cbr_flag[32];
	int CpbCnt = cpb_cnt_minus1[subLayerId]+1;
	for( i = 0; i < CpbCnt; i++ ) {
		bit_rate_value_minus1[ i ] = h265_ue(buf,&bitpos);
		cpb_size_value_minus1[ i ] = h265_ue(buf,&bitpos);
		if( sub_pic_hrd_params_present_flag ) {
			cpb_size_du_value_minus1[ i ] = h265_ue(buf,&bitpos);
			bit_rate_du_value_minus1[ i ] = h265_ue(buf,&bitpos);
		}
		cbr_flag[ i ] = h265_u(buf, &bitpos, 1);
	}
	return bitpos;
}


/*
hrd_parameters( commonInfPresentFlag, maxNumSubLayersMinus1 ) {
	if( commonInfPresentFlag ) {
		nal_hrd_parameters_present_flag u(1)
		vcl_hrd_parameters_present_flag u(1)
		if( nal_hrd_parameters_present_flag | | vcl_hrd_parameters_present_flag ) {
			sub_pic_hrd_params_present_flag u(1)
			if( sub_pic_hrd_params_present_flag ) {
				tick_divisor_minus2 u(8)
				du_cpb_removal_delay_increment_length_minus1 u(5)
				sub_pic_cpb_params_in_pic_timing_sei_flag u(1)
				dpb_output_delay_du_length_minus1 u(5)
			}
			bit_rate_scale u(4)
			cpb_size_scale u(4)
			if( sub_pic_hrd_params_present_flag ){
				cpb_size_du_scale u(4)
			}
			initial_cpb_removal_delay_length_minus1 u(5)
			au_cpb_removal_delay_length_minus1 u(5)
			dpb_output_delay_length_minus1 u(5)
		}
	}

	for( i = 0; i <= maxNumSubLayersMinus1; i++ ) {
		fixed_pic_rate_general_flag[ i ] u(1)
		if( !fixed_pic_rate_general_flag[ i ] ){
			fixed_pic_rate_within_cvs_flag[ i ] u(1)
		}
		if( fixed_pic_rate_within_cvs_flag[ i ] ){
			elemental_duration_in_tc_minus1[ i ] ue(v)
		}
		else{
			low_delay_hrd_flag[ i ] u(1)
		}

		if( !low_delay_hrd_flag[ i ] ){
			cpb_cnt_minus1[ i ] ue(v)
		}
		if( nal_hrd_parameters_present_flag ){
			sub_layer_hrd_parameters( i )
		}
		if( vcl_hrd_parameters_present_flag ){
			sub_layer_hrd_parameters( i )
		}
	}
}
*/
int hrd_parameters(
	u8* buf, int bitpos,
	int commonInfPresentFlag, int maxNumSubLayersMinus1 )
{
printf("sps_vui_hrd{\n");
	u8 nal_hrd_parameters_present_flag = 0;
	u8 vcl_hrd_parameters_present_flag = 0;
	u8 sub_pic_hrd_params_present_flag = 0;
	if( commonInfPresentFlag ) {
		nal_hrd_parameters_present_flag = h265_u(buf,&bitpos,1);
		vcl_hrd_parameters_present_flag = h265_u(buf,&bitpos,1);
		if( nal_hrd_parameters_present_flag | vcl_hrd_parameters_present_flag ) {
			u8 sub_pic_hrd_params_present_flag = h265_u(buf,&bitpos,1);
			if( sub_pic_hrd_params_present_flag ) {
				u8 tick_divisor_minus2 = h265_u(buf,&bitpos,8);
				u8 du_cpb_removal_delay_increment_length_minus1 = h265_u(buf,&bitpos,5);
				u8 sub_pic_cpb_params_in_pic_timing_sei_flag = h265_u(buf,&bitpos,1);
				u8 dpb_output_delay_du_length_minus1 = h265_u(buf,&bitpos,5);
			}
			u8 bit_rate_scale = h265_u(buf,&bitpos,4);
			u8 cpb_size_scale = h265_u(buf,&bitpos,4);
			if( sub_pic_hrd_params_present_flag ){
				u8 cpb_size_du_scale = h265_u(buf,&bitpos,4);
			}
			u8 initial_cpb_removal_delay_length_minus1 = h265_u(buf,&bitpos,5);
			u8 au_cpb_removal_delay_length_minus1 = h265_u(buf,&bitpos,5);
			u8 dpb_output_delay_length_minus1 = h265_u(buf,&bitpos,5);
		}
	}

	u8 fixed_pic_rate_general_flag[32];
	u8 fixed_pic_rate_within_cvs_flag[32];
	u32 elemental_duration_in_tc_minus1[32];
	u8 low_delay_hrd_flag[32];
	u32 cpb_cnt_minus1[32];
	int i;
	for( i = 0; i <= maxNumSubLayersMinus1; i++ ) {
		fixed_pic_rate_general_flag[ i ] = h265_u(buf,&bitpos,1);
		if( !fixed_pic_rate_general_flag[ i ] ){
			fixed_pic_rate_within_cvs_flag[ i ] = h265_u(buf,&bitpos,1);
		}
		if( fixed_pic_rate_within_cvs_flag[ i ] ){
			elemental_duration_in_tc_minus1[ i ] = h265_ue(buf,&bitpos);
		}
		else{
			low_delay_hrd_flag[ i ] = h265_u(buf,&bitpos,1);
		}

		if( !low_delay_hrd_flag[ i ] ){
			cpb_cnt_minus1[ i ] = h265_ue(buf,&bitpos);
		}
		if( nal_hrd_parameters_present_flag ){
			bitpos = sub_layer_hrd_parameters(buf,bitpos,
				i, sub_pic_hrd_params_present_flag,
				cpb_cnt_minus1);
		}
		if( vcl_hrd_parameters_present_flag ){
			bitpos = sub_layer_hrd_parameters(buf,bitpos,
				i , sub_pic_hrd_params_present_flag,
				cpb_cnt_minus1);
		}
	}
printf("}sps_vui_hrd\n");
	return bitpos;
}



/*
video_parameter_set_rbsp( ) {
	vps_video_parameter_set_id u(4)
	vps_base_layer_internal_flag u(1)
	vps_base_layer_available_flag u(1)
	vps_max_layers_minus1 u(6)
	vps_max_sub_layers_minus1 u(3)
	vps_temporal_id_nesting_flag u(1)
	vps_reserved_0xffff_16bits u(16)
	profile_tier_level( 1, vps_max_sub_layers_minus1 )
	vps_sub_layer_ordering_info_present_flag u(1)
	for( i = ( vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1 );i <= vps_max_sub_layers_minus1; i++ ) {
		vps_max_dec_pic_buffering_minus1[ i ] ue(v)
		vps_max_num_reorder_pics[ i ] ue(v)
		vps_max_latency_increase_plus1[ i ] ue(v)
	}

	vps_max_layer_id u(6)
	vps_num_layer_sets_minus1 ue(v)
	for( i = 1; i <= vps_num_layer_sets_minus1; i++ ){
		for( j = 0; j <= vps_max_layer_id; j++ ){
			layer_id_included_flag[ i ][ j ] u(1)
		}
	}

	vps_timing_info_present_flag u(1)
	if( vps_timing_info_present_flag ) {
		vps_num_units_in_tick u(32)
		vps_time_scale u(32)

		vps_poc_proportional_to_timing_flag u(1)
		if( vps_poc_proportional_to_timing_flag ){
			vps_num_ticks_poc_diff_one_minus1 ue(v)
		}

		vps_num_hrd_parameters ue(v)
		for( i = 0; i < vps_num_hrd_parameters; i++ ) {
			hrd_layer_set_idx[ i ] ue(v)
			if( i > 0 ){
				cprms_present_flag[ i ] u(1)
			}
			hrd_parameters( cprms_present_flag[ i ], vps_max_sub_layers_minus1 )
		}
	}

	vps_extension_flag u(1)
	if( vps_extension_flag ){
		while( more_rbsp_data( ) ){
			vps_extension_data_flag u(1)
		}
	}

	rbsp_trailing_bits( )
}*/
struct parsed_vps{
	u32 vps_video_parameter_set_id;
	u32 vps_base_layer_internal_flag;
	u32 vps_base_layer_available_flag;
	u32 vps_max_layers_minus1;
	u32 vps_max_sub_layers_minus1;
	u32 vps_temporal_id_nesting_flag;
	//profile_tier_level( 1, vps_max_sub_layers_minus1 )
	u32 vps_sub_layer_ordering_info_present_flag;
	u32 vps_max_dec_pic_buffering_minus1[8];
	u32 vps_max_num_reorder_pics[8];
	u32 vps_max_latency_increase_plus1[8];
	u32 vps_max_layer_id;
	u32 vps_num_layer_sets_minus1;
	u32 layer_id_included_flag[8][8];
	u32 vps_timing_info_present_flag;
	u32 vps_num_units_in_tick;
	u32 vps_time_scale;
	u32 vps_poc_proportional_to_timing_flag;
	u32 vps_num_ticks_poc_diff_one_minus1;
	u32 vps_num_hrd_parameters;
	u32 hrd_layer_set_idx[8];
	u32 cprms_present_flag[8];
	//hrd_parameters( cprms_present_flag[ i ], vps_max_sub_layers_minus1 )
	u32 vps_extension_flag;
	u32 vps_extension_data_flag;
};
static struct parsed_vps pervps;
void parseh265_vps(u8* buf, int len)
{
printf("vps{\n");
	int i,j;
	int bitpos = 16;

	u8 vps_video_parameter_set_id = h265_u(buf, &bitpos,4);
	printf("vps_video_parameter_set_id = %x\n", vps_video_parameter_set_id);
	u8 vps_base_layer_internal_flag = h265_u(buf, &bitpos,1);
	u8 vps_base_layer_available_flag = h265_u(buf, &bitpos,1);
	u8 vps_max_layers_minus1 = h265_u(buf, &bitpos,6);
	printf("vps_max_layers_minus1 = %x\n",vps_max_layers_minus1);
	u8 vps_max_sub_layers_minus1 = h265_u(buf, &bitpos,3);
	printf("vps_max_sub_layers_minus1 = %x\n",vps_max_sub_layers_minus1);
	u8 vps_temporal_id_nesting_flag = h265_u(buf, &bitpos,1);
	bitpos += 16;

	bitpos = profile_tier_level(buf, bitpos, 1, vps_max_sub_layers_minus1);

	u8 vps_sub_layer_ordering_info_present_flag = h265_u(buf, &bitpos,1);
	u32 vps_max_dec_pic_buffering_minus1[32];
	u32 vps_max_num_reorder_pics[32];
	u32 vps_max_latency_increase_plus1[32];
	for( i = ( vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1 );i <= vps_max_sub_layers_minus1; i++ ) {
		vps_max_dec_pic_buffering_minus1[ i ] = h265_ue(buf, &bitpos);
		vps_max_num_reorder_pics[ i ] = h265_ue(buf, &bitpos);
		vps_max_latency_increase_plus1[ i ] = h265_ue(buf, &bitpos);
	}

	u8 vps_max_layer_id = h265_u(buf, &bitpos,6);
	printf("vps_max_layer_id = %x\n", vps_max_layer_id);
	u32 vps_num_layer_sets_minus1 = h265_ue(buf, &bitpos);
	printf("vps_num_layer_sets_minus1 = %x\n",vps_num_layer_sets_minus1);
	u8 layer_id_included_flag[32][32];
	for( i = 1; i <= vps_num_layer_sets_minus1; i++ ){
		for( j = 0; j <= vps_max_layer_id; j++ ){
			layer_id_included_flag[ i ][ j ] = h265_u(buf, &bitpos,1);
		}
	}

	u8 vps_timing_info_present_flag = h265_u(buf, &bitpos,1);
	if( vps_timing_info_present_flag ) {
		u32 vps_num_units_in_tick = h265_u(buf, &bitpos,32);
		printf("vps_num_units_in_tick = %x\n", vps_num_units_in_tick);
		u32 vps_time_scale = h265_u(buf, &bitpos,32);
		printf("vps_time_scale = %x\n", vps_time_scale);

		u8 vps_poc_proportional_to_timing_flag = h265_u(buf, &bitpos,1);
		if( vps_poc_proportional_to_timing_flag ){
			u32 vps_num_ticks_poc_diff_one_minus1 = h265_ue(buf, &bitpos);
		}

		u32 vps_num_hrd_parameters = h265_ue(buf, &bitpos);
		u32 hrd_layer_set_idx[32];
		u8 cprms_present_flag[32];
		for( i = 0; i < vps_num_hrd_parameters; i++ ) {
			hrd_layer_set_idx[ i ] = h265_ue(buf, &bitpos);
			if( i > 0 ){
				cprms_present_flag[ i ] = h265_u(buf, &bitpos,1);
			}
			bitpos = hrd_parameters(buf, bitpos, cprms_present_flag[ i ], vps_max_sub_layers_minus1 );
		}
	}

	u8 vps_extension_flag = h265_u(buf, &bitpos,1);

printf("}vps\n\n");
}


/*
sps_range_extension( ) { 
	transform_skip_rotation_enabled_flag u(1)
	transform_skip_context_enabled_flag u(1)
	implicit_rdpcm_enabled_flag u(1)
	explicit_rdpcm_enabled_flag u(1)
	extended_precision_processing_flag u(1)
	intra_smoothing_disabled_flag u(1)
	high_precision_offsets_enabled_flag u(1)
	persistent_rice_adaptation_enabled_flag u(1)
	cabac_bypass_alignment_enabled_flag u(1)
}*/


/*
st_ref_pic_set( stRpsIdx ) {
	if( stRpsIdx != 0 ){
		inter_ref_pic_set_prediction_flag	u(1)
	}
	if( inter_ref_pic_set_prediction_flag ) {
		if( stRpsIdx = = num_short_term_ref_pic_sets ){
			delta_idx_minus1		ue(v)
		}
		delta_rps_sign				u(1)
		abs_delta_rps_minus1			ue(v)
		for( j = 0; j <= NumDeltaPocs[ RefRpsIdx ]; j++ ) {
			used_by_curr_pic_flag[ j ]		u(1)
			if( !used_by_curr_pic_flag[ j ] ){
				use_delta_flag[ j ]		u(1)
			}
		}
	} else {
		num_negative_pics		ue(v)
		num_positive_pics		ue(v)
		for( i = 0; i < num_negative_pics; i++ ) {
			delta_poc_s0_minus1[ i ]		ue(v)
			used_by_curr_pic_s0_flag[ i ]		u(1)
		}
		for( i = 0; i < num_positive_pics; i++ ) {
			delta_poc_s1_minus1[ i ]		ue(v)
			used_by_curr_pic_s1_flag[ i ]		u(1)
		}
	}
}*/
int st_ref_pic_set(
	u8* buf, int bitpos,
	int stRpsIdx, int num_short_term_ref_pic_sets, int* num_delta_pocs)
{
	int j;
	u8 inter_ref_pic_set_prediction_flag = 0;
	if( stRpsIdx != 0 ){
		inter_ref_pic_set_prediction_flag = h265_u(buf, &bitpos, 1);
	}

	if( inter_ref_pic_set_prediction_flag ) {
		if( stRpsIdx == num_short_term_ref_pic_sets ){
			int delta_idx_minus1 = h265_ue(buf, &bitpos);
		}
		u8 delta_rps_sign = h265_u(buf, &bitpos, 1);
		int abs_delta_rps_minus1 = h265_ue(buf, &bitpos);

		num_delta_pocs[stRpsIdx] = 0;
		for (j = 0; j <= num_delta_pocs[stRpsIdx - 1]; j++) {
			u8 use_delta_flag = 0;
			u8 used_by_curr_pic_flag = h265_u(buf, &bitpos, 1);
			if (!used_by_curr_pic_flag){
				use_delta_flag = h265_u(buf, &bitpos, 1);
			}
			if (used_by_curr_pic_flag | use_delta_flag){
				num_delta_pocs[stRpsIdx]++;
			}
		}
	} else {
		int delta_poc_s0_minus1[32];
		int used_by_curr_pic_s0_flag[32];
		int delta_poc_s1_minus1[32];
		int used_by_curr_pic_s1_flag[32];

		u32 num_negative_pics = h265_ue(buf, &bitpos);
		u32 num_positive_pics = h265_ue(buf, &bitpos);
		num_delta_pocs[ stRpsIdx ] = num_negative_pics+num_positive_pics;
		for( j = 0; j < num_negative_pics; j++ ) {
			delta_poc_s0_minus1[ j ] = h265_ue(buf, &bitpos);
			used_by_curr_pic_s0_flag[ j ] = h265_u(buf, &bitpos, 1);
		}
		for( j = 0; j < num_positive_pics; j++ ) {
			delta_poc_s1_minus1[ j ] = h265_ue(buf, &bitpos);
			used_by_curr_pic_s1_flag[ j ] = h265_u(buf, &bitpos, 1);
		}
	}
	return bitpos;
}



/*
vui_parameters( ) {
	aspect_ratio_info_present_flag u(1)
	if( aspect_ratio_info_present_flag ) {
		aspect_ratio_idc u(8)
		if( aspect_ratio_idc = = EXTENDED_SAR ) {
			sar_width u(16)
			sar_height u(16)
		}
	}

	overscan_info_present_flag u(1)
	if( overscan_info_present_flag ){
		overscan_appropriate_flag u(1)
	}

	video_signal_type_present_flag u(1)
	if( video_signal_type_present_flag ) {
		video_format u(3)
		video_full_range_flag u(1)

		colour_description_present_flag u(1)
		if( colour_description_present_flag ) {
			colour_primaries u(8)
			transfer_characteristics u(8)
			matrix_coeffs u(8)
		}
	}

	chroma_loc_info_present_flag u(1)
	if( chroma_loc_info_present_flag ) {
		chroma_sample_loc_type_top_field ue(v)
		chroma_sample_loc_type_bottom_field ue(v)
	}

	neutral_chroma_indication_flag u(1)
	field_seq_flag u(1)
	frame_field_info_present_flag u(1)
	default_display_window_flag u(1)
	if( default_display_window_flag ) {
		def_disp_win_left_offset ue(v)
		def_disp_win_right_offset ue(v)
		def_disp_win_top_offset ue(v)
		def_disp_win_bottom_offset ue(v)
	}

	vui_timing_info_present_flag u(1)
	if( vui_timing_info_present_flag ) {
		vui_num_units_in_tick u(32)
		vui_time_scale u(32)
		vui_poc_proportional_to_timing_flag u(1)
		if( vui_poc_proportional_to_timing_flag ){
			vui_num_ticks_poc_diff_one_minus1 ue(v)
		}

		vui_hrd_parameters_present_flag u(1)
		if( vui_hrd_parameters_present_flag ){
			hrd_parameters( 1, sps_max_sub_layers_minus1 )
		}

		bitstream_restriction_flag u(1)
		if( bitstream_restriction_flag ) {
			tiles_fixed_structure_flag u(1)
			motion_vectors_over_pic_boundaries_flag u(1)
			restricted_ref_pic_lists_flag u(1)
			min_spatial_segmentation_idc ue(v)
			max_bytes_per_pic_denom ue(v)
			max_bits_per_min_cu_denom ue(v)
			log2_max_mv_length_horizontal ue(v)
			log2_max_mv_length_vertical ue(v)
		}
	}
}
*/
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
	u32 matrix_coeffs;
	u32 chroma_loc_info_present_flag;
	u32 chroma_sample_loc_type_top_field;
	u32 chroma_sample_loc_type_bottom_field;
	u32 neutral_chroma_indication_flag;
	u32 field_seq_flag;
	u32 frame_field_info_present_flag;
	u32 default_display_window_flag;
	u32 def_disp_win_left_offset;
	u32 def_disp_win_right_offset;
	u32 def_disp_win_top_offset;
	u32 def_disp_win_bottom_offset;
	u32 vui_timing_info_present_flag;
	u32 vui_num_units_in_tick;
	u32 vui_time_scale;
	u32 vui_poc_proportional_to_timing_flag;
	u32 vui_num_ticks_poc_diff_one_minus1;
	u32 vui_hrd_parameters_present_flag;
	//hrd_parameters( 1, sps_max_sub_layers_minus1 )
	u32 bitstream_restriction_flag;
	u32 tiles_fixed_structure_flag;
	u32 motion_vectors_over_pic_boundaries_flag;
	u32 restricted_ref_pic_lists_flag;
	u32 min_spatial_segmentation_idc;
	u32 max_bytes_per_pic_denom;
	u32 max_bits_per_min_cu_denom;
	u32 log2_max_mv_length_horizontal;
	u32 log2_max_mv_length_vertical;
};
static struct parsed_vui pervui;
int parseh265_sps_vui(u8* buf, int bitpos,
	int sps_max_sub_layers_minus1)
{
printf("sps_vui{\n");
	u8 aspect_ratio_info_present_flag = h265_u(buf, &bitpos, 1);
	printf("aspect_ratio_info_present_flag = %d\n", aspect_ratio_info_present_flag);
	if( aspect_ratio_info_present_flag ) {
		u8 aspect_ratio_idc = h265_u(buf, &bitpos, 8);
		printf("aspect_ratio_idc = %d\n", aspect_ratio_idc);
		if( aspect_ratio_idc == 255) {
			int sar_width = h265_u(buf, &bitpos, 16);
			int sar_height = h265_u(buf, &bitpos, 16);
			printf("sar_width = %d\n", sar_width);
			printf("sar_height = %d\n", sar_height);
		}
	}

	u8 overscan_info_present_flag = h265_u(buf,&bitpos,1);
	if( overscan_info_present_flag ){
		u8 overscan_appropriate_flag = h265_u(buf,&bitpos,1);
	}

	u8 video_signal_type_present_flag = h265_u(buf,&bitpos,1);
	if( video_signal_type_present_flag ) {
		u8 video_format = h265_u(buf,&bitpos,3);
		u8 video_full_range_flag = h265_u(buf,&bitpos,1);

		u8 colour_description_present_flag = h265_u(buf,&bitpos,1);
		if( colour_description_present_flag ) {
			u8 colour_primaries = h265_u(buf,&bitpos,8);
			printf("colour_primaries = %x\n", colour_primaries);
			u8 transfer_characteristics = h265_u(buf,&bitpos,8);
			printf("transfer_characteristics = %x\n", transfer_characteristics);
			u8 matrix_coeffs = h265_u(buf,&bitpos,8);
			printf("matrix_coeffs = %x\n", matrix_coeffs);
		}
	}

	u8 chroma_loc_info_present_flag = h265_u(buf,&bitpos,1);
	if( chroma_loc_info_present_flag ) {
		u32 chroma_sample_loc_type_top_field = h265_ue(buf,&bitpos);
		u32 chroma_sample_loc_type_bottom_field = h265_ue(buf,&bitpos);
	}

	u8 neutral_chroma_indication_flag = h265_u(buf,&bitpos,1);
	u8 field_seq_flag = h265_u(buf,&bitpos,1);
	u8 frame_field_info_present_flag = h265_u(buf,&bitpos,1);
	u8 default_display_window_flag = h265_u(buf,&bitpos,1);
	if( default_display_window_flag ) {
		u32 def_disp_win_left_offset = h265_ue(buf,&bitpos);
		u32 def_disp_win_right_offset = h265_ue(buf,&bitpos);
		u32 def_disp_win_top_offset = h265_ue(buf,&bitpos);
		u32 def_disp_win_bottom_offset = h265_ue(buf,&bitpos);
	}

	u8 vui_timing_info_present_flag = h265_u(buf,&bitpos,1);
	if( vui_timing_info_present_flag ) {
		u32 vui_num_units_in_tick = h265_u(buf,&bitpos,32);
		u32 vui_time_scale = h265_u(buf,&bitpos,32);
		printf("vui_time_scale = %d\n",vui_time_scale);

		u8 vui_poc_proportional_to_timing_flag = h265_u(buf,&bitpos,1);
		if( vui_poc_proportional_to_timing_flag ){
			u32 vui_num_ticks_poc_diff_one_minus1 = h265_ue(buf,&bitpos);
		}

		u8 vui_hrd_parameters_present_flag = h265_u(buf,&bitpos,1);
		if( vui_hrd_parameters_present_flag ){
			bitpos = hrd_parameters(buf,bitpos,
				1, sps_max_sub_layers_minus1 );
		}

		u8 bitstream_restriction_flag = h265_u(buf,&bitpos,1);
		if( bitstream_restriction_flag ) {
			u8 tiles_fixed_structure_flag = h265_u(buf,&bitpos,1);
			u8 motion_vectors_over_pic_boundaries_flag = h265_u(buf,&bitpos,1);
			u8 restricted_ref_pic_lists_flag = h265_u(buf,&bitpos,1);
			u32 min_spatial_segmentation_idc = h265_ue(buf,&bitpos);
			u32 max_bytes_per_pic_denom = h265_ue(buf,&bitpos);
			u32 max_bits_per_min_cu_denom = h265_ue(buf,&bitpos);
			u32 log2_max_mv_length_horizontal = h265_ue(buf,&bitpos);
			u32 log2_max_mv_length_vertical = h265_ue(buf,&bitpos);
		}
	}
printf("}sps_vui\n");
	return bitpos;
}




/*
seq_parameter_set_rbsp( ) {
	sps_video_parameter_set_id			u(4)
	sps_max_sub_layers_minus1			u(3)
	sps_temporal_id_nesting_flag			u(1)
	//profile_tier_level( 1, sps_max_sub_layers_minus1 )
	sps_seq_parameter_set_id			ue(v)

	chroma_format_idc				ue(v)
	if( chroma_format_idc = = 3 ){
		separate_colour_plane_flag		u(1)
	}	

	pic_width_in_luma_samples			ue(v)
	pic_height_in_luma_samples			ue(v)

	conformance_window_flag				u(1)
	if( conformance_window_flag ) {
		conf_win_left_offset			ue(v)
		conf_win_right_offset			ue(v)
		conf_win_top_offset			ue(v)
		conf_win_bottom_offset			ue(v)
	}

	bit_depth_luma_minus8				ue(v)
	bit_depth_chroma_minus8				ue(v)
	log2_max_pic_order_cnt_lsb_minus4		ue(v)
	sps_sub_layer_ordering_info_present_flag	u(1)
	for( i = ( sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1 );i <= sps_max_sub_layers_minus1; i++ ) {
		sps_max_dec_pic_buffering_minus1[i]	ue(v)
		sps_max_num_reorder_pics[i]		ue(v)
		sps_max_latency_increase_plus1[i]	ue(v)
	}

	log2_min_luma_coding_block_size_minus3		ue(v)
	log2_diff_max_min_luma_coding_block_size	ue(v)
	log2_min_luma_transform_block_size_minus2	ue(v)
	log2_diff_max_min_luma_transform_block_size	ue(v)
	max_transform_hierarchy_depth_inter		ue(v)
	max_transform_hierarchy_depth_intra		ue(v)

	scaling_list_enabled_flag			u(1)
	if( scaling_list_enabled_flag ) {
		sps_scaling_list_data_present_flag	u(1)
		if( sps_scaling_list_data_present_flag ){
			scaling_list_data()
		}
	}

	amp_enabled_flag				u(1)
	sample_adaptive_offset_enabled_flag		u(1)

	pcm_enabled_flag				u(1)
	if( pcm_enabled_flag ) {
		pcm_sample_bit_depth_luma_minus1	u(4)
		pcm_sample_bit_depth_chroma_minus1	u(4)
		log2_min_pcm_luma_coding_block_size_minus3	ue(v)
		log2_diff_max_min_pcm_luma_coding_block_size	ue(v)
		pcm_loop_filter_disabled_flag		u(1)
	}

	num_short_term_ref_pic_sets			ue(v)
	for( i = 0; i < num_short_term_ref_pic_sets; i++){
		st_ref_pic_set( i )
	}

	long_term_ref_pics_present_flag			u(1)
	if( long_term_ref_pics_present_flag ) {
		num_long_term_ref_pics_sps		ue(v)
		for( i = 0; i < num_long_term_ref_pics_sps; i++ ) {
			lt_ref_pic_poc_lsb_sps[ i ]	u(v)
			used_by_curr_pic_lt_sps_flag[ i ]	u(1)
		}
	}

	sps_temporal_mvp_enabled_flag			u(1)
	strong_intra_smoothing_enabled_flag		u(1)
	vui_parameters_present_flag			u(1)
	if( vui_parameters_present_flag ){
		vui_parameters( )
	}

	sps_extension_present_flag			u(1)
	if( sps_extension_present_flag ) {
		sps_range_extension_flag		u(1)
		sps_multilayer_extension_flag		u(1)
		sps_3d_extension_flag			u(1)
		sps_scc_extension_flag			u(1)
		sps_extension_4bits			u(4)
	}

	if( sps_range_extension_flag ){
		sps_range_extension( )
	}

	if( sps_multilayer_extension_flag ){
		sps_multilayer_extension( ) // specified in Annex F
	}

	if( sps_3d_extension_flag ){
		sps_3d_extension( ) // specified in Annex I
	}

	if( sps_scc_extension_flag ){
		sps_scc_extension( )
	}

	if( sps_extension_4bits ){
		while( more_rbsp_data( ) ){
			sps_extension_data_flag		u(1)
		}
	}

	rbsp_trailing_bits( )
}*/
struct parsed_sps{
	u32 sps_video_parameter_set_id;
	u32 sps_max_sub_layers_minus1;
	u32 sps_temporal_id_nesting_flag;
	//profile_tier_level( 1, sps_max_sub_layers_minus1 )
	u32 sps_seq_parameter_set_id;
	u32 chroma_format_idc;
	u32 separate_colour_plane_flag;
	u32 pic_width_in_luma_samples;
	u32 pic_height_in_luma_samples;
	u32 conformance_window_flag;
	u32 conf_win_left_offset;
	u32 conf_win_right_offset;
	u32 conf_win_top_offset;
	u32 conf_win_bottom_offset;
	u32 bit_depth_luma_minus8;
	u32 bit_depth_chroma_minus8;
	u32 log2_max_pic_order_cnt_lsb_minus4;
	u32 sps_sub_layer_ordering_info_present_flag;
	u32 sps_max_dec_pic_buffering_minus1[8];
	u32 sps_max_num_reorder_pics[8];
	u32 sps_max_latency_increase_plus1[8];
	u32 log2_min_luma_coding_block_size_minus3;
	u32 log2_diff_max_min_luma_coding_block_size;
	u32 log2_min_luma_transform_block_size_minus2;
	u32 log2_diff_max_min_luma_transform_block_size;
	u32 max_transform_hierarchy_depth_inter;
	u32 max_transform_hierarchy_depth_intra;
	u32 scaling_list_enabled_flag;
	u32 sps_scaling_list_data_present_flag;
	u32 amp_enabled_flag;
	u32 sample_adaptive_offset_enabled_flag;
	u32 pcm_enabled_flag;
	u32 pcm_sample_bit_depth_luma_minus1;
	u32 pcm_sample_bit_depth_chroma_minus1;
	u32 log2_min_pcm_luma_coding_block_size_minus3;
	u32 log2_diff_max_min_pcm_luma_coding_block_size;
	u32 pcm_loop_filter_disabled_flag;
	u32 num_short_term_ref_pic_sets;
	//st_ref_pic_set( i )
	u32 long_term_ref_pics_present_flag;
	u32 num_long_term_ref_pics_sps;
	u32 lt_ref_pic_poc_lsb_sps[8];
	u32 used_by_curr_pic_lt_sps_flag[8];
	u32 sps_temporal_mvp_enabled_flag;
	u32 strong_intra_smoothing_enabled_flag;
	u32 vui_parameters_present_flag;
	//vui_parameters( )
	u32 sps_extension_present_flag;
	u32 sps_range_extension_flag;
	u32 sps_multilayer_extension_flag;
	u32 sps_3d_extension_flag;
	u32 sps_scc_extension_flag;
	u32 sps_extension_4bits;
	//sps_range_extension( )
	//sps_multilayer_extension( ) // specified in Annex F
	//sps_3d_extension( ) // specified in Annex I
	//sps_scc_extension( )
	u32 sps_extension_data_flag;
};
static struct parsed_sps persps;
void parseh265_sps(u8* buf, int len)
{
printf("sps{\n");
	int bitpos = 16;
	int sps_video_parameter_set_id = h265_u(buf, &bitpos, 4);
	printf("sps_video_parameter_set_id = %x\n",sps_video_parameter_set_id);

	int sps_max_sub_layers_minus1 = h265_u(buf, &bitpos, 3);
	printf("sps_max_sub_layers_minus1 = %x\n", sps_max_sub_layers_minus1);

	int sps_temporal_id_nesting_flag = h265_u(buf, &bitpos, 1);
	printf("sps_temporal_id_nesting_flag = %x\n", sps_temporal_id_nesting_flag);

	bitpos = profile_tier_level(buf, bitpos, 1, sps_max_sub_layers_minus1);

	int sps_seq_parameter_set_id = h265_ue(buf, &bitpos);
	printf("sps_seq_parameter_set_id = %x\n",sps_seq_parameter_set_id);

	int chroma_format_idc = h265_ue(buf, &bitpos);
	printf("chroma_format_idc = %x\n", chroma_format_idc);
	if( chroma_format_idc == 3 ){
		int separate_colour_plane_flag = h265_u(buf, &bitpos, 1);
		printf("separate_colour_plane_flag = %x\n", separate_colour_plane_flag);
	}	

	int pic_width_in_luma_samples = h265_ue(buf, &bitpos);
	printf("pic_width_in_luma_samples = %d\n", pic_width_in_luma_samples);
	int pic_height_in_luma_samples = h265_ue(buf, &bitpos);
	printf("pic_height_in_luma_samples = %d\n", pic_height_in_luma_samples);

	u8 conformance_window_flag = h265_u(buf, &bitpos, 1);
	if( conformance_window_flag ) {
		int conf_win_left_offset = h265_ue(buf, &bitpos);
		printf("conf_win_left_offset = %x\n",conf_win_left_offset);
		int conf_win_right_offset = h265_ue(buf, &bitpos);
		printf("conf_win_right_offset = %x\n",conf_win_right_offset);
		int conf_win_top_offset = h265_ue(buf, &bitpos);
		printf("conf_win_top_offset = %x\n",conf_win_top_offset);
		int conf_win_bottom_offset = h265_ue(buf, &bitpos);
		printf("conf_win_bottom_offset = %x\n",conf_win_bottom_offset);
	}

	int bit_depth_luma_minus8 = h265_ue(buf, &bitpos);
	printf("bit_depth_luma_minus8 = %x\n",bit_depth_luma_minus8);
	int bit_depth_chroma_minus8 = h265_ue(buf, &bitpos);
	printf("bit_depth_chroma_minus8 = %x\n",bit_depth_chroma_minus8);
	int log2_max_pic_order_cnt_lsb_minus4 = h265_ue(buf, &bitpos);
	printf("log2_max_pic_order_cnt_lsb_minus4 = %x\n",log2_max_pic_order_cnt_lsb_minus4);

	int j;
	int sps_max_dec_pic_buffering_minus1[32];
	int sps_max_num_reorder_pics[32];
	int sps_max_latency_increase_plus1[32];
	u8 sps_sub_layer_ordering_info_present_flag = h265_u(buf, &bitpos, 1);
	for(j = ( sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1 );j <= sps_max_sub_layers_minus1; j++ ) {
		sps_max_dec_pic_buffering_minus1[j] = h265_ue(buf, &bitpos);
		sps_max_num_reorder_pics[j] = h265_ue(buf, &bitpos);
		sps_max_latency_increase_plus1[j] = h265_ue(buf, &bitpos);
	}

	int log2_min_luma_coding_block_size_minus3 = h265_ue(buf, &bitpos);
	int log2_diff_max_min_luma_coding_block_size = h265_ue(buf, &bitpos);
	int log2_min_luma_transform_block_size_minus2 = h265_ue(buf, &bitpos);
	int log2_diff_max_min_luma_transform_block_size = h265_ue(buf, &bitpos);
	int max_transform_hierarchy_depth_inter = h265_ue(buf, &bitpos);
	int max_transform_hierarchy_depth_intra = h265_ue(buf, &bitpos);

	u8 scaling_list_enabled_flag = h265_u(buf, &bitpos, 1);
	if( scaling_list_enabled_flag ) {
		u8 sps_scaling_list_data_present_flag;
		if( sps_scaling_list_data_present_flag ){
			bitpos = scaling_list_data(buf, bitpos);
		}
	}

	u8 amp_enabled_flag = h265_u(buf, &bitpos, 1);
	u8 sample_adaptive_offset_enabled_flag = h265_u(buf, &bitpos, 1);

	u8 pcm_enabled_flag = h265_u(buf, &bitpos, 1);
	if( pcm_enabled_flag ) {
		u8 pcm_sample_bit_depth_luma_minus1 = h265_u(buf, &bitpos, 4);
		u8 pcm_sample_bit_depth_chroma_minus1 = h265_u(buf, &bitpos, 4);
		int log2_min_pcm_luma_coding_block_size_minus3 = h265_ue(buf, &bitpos);
		int log2_diff_max_min_pcm_luma_coding_block_size = h265_ue(buf, &bitpos);
		u8 pcm_loop_filter_disabled_flag = h265_u(buf, &bitpos, 1);
	}

	int num_short_term_ref_pic_sets = h265_ue(buf, &bitpos);
	printf("num_short_term_ref_pic_sets = %x\n",num_short_term_ref_pic_sets);
	int num_delta_pocs[32];
	for(j = 0; j < num_short_term_ref_pic_sets; j++){
		bitpos = st_ref_pic_set(buf, bitpos,
			j, num_short_term_ref_pic_sets, num_delta_pocs);
	}

	u8 long_term_ref_pics_present_flag = h265_u(buf, &bitpos, 1);
	u32 lt_ref_pic_poc_lsb_sps[32];
	u8 used_by_curr_pic_lt_sps_flag[32];
	if( long_term_ref_pics_present_flag ) {
		int num_long_term_ref_pics_sps = h265_ue(buf, &bitpos);
		printf("num_long_term_ref_pics_sps = %x\n",num_long_term_ref_pics_sps);
		for(j = 0; j < num_long_term_ref_pics_sps; j++ ) {
			int var = log2_max_pic_order_cnt_lsb_minus4 + 4;
			if(var > 16)var = 16;
			lt_ref_pic_poc_lsb_sps[j] = h265_u(buf, &bitpos, var);
			used_by_curr_pic_lt_sps_flag[j] = h265_u(buf, &bitpos, 1);
		}
	}

	u8 sps_temporal_mvp_enabled_flag = h265_u(buf, &bitpos, 1);
	u8 strong_intra_smoothing_enabled_flag = h265_u(buf, &bitpos, 1);

	u8 vui_parameters_present_flag = h265_u(buf, &bitpos, 1);
	printf("vui_parameters_present_flag = %x\n",vui_parameters_present_flag);
	if( vui_parameters_present_flag ){
		bitpos = parseh265_sps_vui(buf, bitpos, sps_max_sub_layers_minus1);
	}

printf("}sps\n\n");
}



/*
pic_parameter_set_rbsp( ) {
	pps_pic_parameter_set_id ue(v)
	pps_seq_parameter_set_id ue(v)
	dependent_slice_segments_enabled_flag u(1)
	output_flag_present_flag u(1)
	num_extra_slice_header_bits u(3)
	sign_data_hiding_enabled_flag u(1)
	cabac_init_present_flag u(1)
	num_ref_idx_l0_default_active_minus1 ue(v)
	num_ref_idx_l1_default_active_minus1 ue(v)
	init_qp_minus26 se(v)
	constrained_intra_pred_flag u(1)
	transform_skip_enabled_flag u(1)
	cu_qp_delta_enabled_flag u(1)
	if( cu_qp_delta_enabled_flag ){
		diff_cu_qp_delta_depth ue(v)
	}

	pps_cb_qp_offset se(v)
	pps_cr_qp_offset se(v)
	pps_slice_chroma_qp_offsets_present_flag u(1)
	weighted_pred_flag u(1)
	weighted_bipred_flag u(1)
	transquant_bypass_enabled_flag u(1)
	tiles_enabled_flag u(1)
	entropy_coding_sync_enabled_flag u(1)
	if( tiles_enabled_flag ) {
		num_tile_columns_minus1 ue(v)
		num_tile_rows_minus1 ue(v)
		uniform_spacing_flag u(1)
		if( !uniform_spacing_flag ) {
			for( i = 0; i < num_tile_columns_minus1; i++ ){
				column_width_minus1[i] ue(v)
			}
			for( i = 0; i < num_tile_rows_minus1; i++ ){
				row_height_minus1[i] ue(v)
			}
		}
		loop_filter_across_tiles_enabled_flag u(1)
	}

	pps_loop_filter_across_slices_enabled_flag u(1)
	deblocking_filter_control_present_flag u(1)
	if( deblocking_filter_control_present_flag ) {
		deblocking_filter_override_enabled_flag u(1)
		pps_deblocking_filter_disabled_flag u(1)
		if( !pps_deblocking_filter_disabled_flag ) {
			pps_beta_offset_div2 se(v)
			pps_tc_offset_div2 se(v)
		}
	}

	pps_scaling_list_data_present_flag u(1)
	if( pps_scaling_list_data_present_flag ){
		scaling_list_data()
	}

	lists_modification_present_flag u(1)
	log2_parallel_merge_level_minus2 ue(v)
	slice_segment_header_extension_present_flag u(1)

	pps_extension_present_flag u(1)
	if( pps_extension_present_flag ) {
		pps_range_extension_flag u(1)
		pps_multilayer_extension_flag u(1)
		pps_3d_extension_flag u(1)
		pps_scc_extension_flag u(1)
		pps_extension_4bits u(4)
	}

	if( pps_range_extension_flag )
		pps_range_extension( )

	if( pps_multilayer_extension_flag )
		pps_multilayer_extension( )		// specified in Annex F

	if( pps_3d_extension_flag )
		pps_3d_extension( )		// specified in Annex I

	if( pps_scc_extension_flag )
		pps_scc_extension( )

	if( pps_extension_4bits )
		while( more_rbsp_data( ) )
			pps_extension_data_flag u(1)

	rbsp_trailing_bits( )
}*/
struct parsed_pps{
	u32 pps_pic_parameter_set_id;
	u32 pps_seq_parameter_set_id;
	u32 dependent_slice_segments_enabled_flag;
	u32 output_flag_present_flag;
	u32 num_extra_slice_header_bits;
	u32 sign_data_hiding_enabled_flag;
	u32 cabac_init_present_flag;
	u32 num_ref_idx_l0_default_active_minus1;
	u32 num_ref_idx_l1_default_active_minus1;
	u32 init_qp_minus26;
	u32 constrained_intra_pred_flag;
	u32 transform_skip_enabled_flag;
	u32 cu_qp_delta_enabled_flag;
	u32 diff_cu_qp_delta_depth;
	u32 pps_cb_qp_offset;
	u32 pps_cr_qp_offset;
	u32 pps_slice_chroma_qp_offsets_present_flag;
	u32 weighted_pred_flag;
	u32 weighted_bipred_flag;
	u32 transquant_bypass_enabled_flag;
	u32 tiles_enabled_flag;
	u32 entropy_coding_sync_enabled_flag;
	u32 num_tile_columns_minus1;
	u32 num_tile_rows_minus1;
	u32 uniform_spacing_flag;
	u32 column_width_minus1[8];
	u32 row_height_minus1[8];
	u32 loop_filter_across_tiles_enabled_flag;
	u32 pps_loop_filter_across_slices_enabled_flag;
	u32 deblocking_filter_control_present_flag;
	u32 deblocking_filter_override_enabled_flag;
	u32 pps_deblocking_filter_disabled_flag;
	u32 pps_beta_offset_div2;
	u32 pps_tc_offset_div2;
	u32 pps_scaling_list_data_present_flag;
	//scaling_list_data()
	u32 lists_modification_present_flag;
	u32 log2_parallel_merge_level_minus2;
	u32 slice_segment_header_extension_present_flag;
	u32 pps_extension_present_flag;
	u32 pps_range_extension_flag;
	u32 pps_multilayer_extension_flag;
};
static struct parsed_pps perpps;
void parseh265_pps(u8* buf, int len)
{
printf("pps{\n");
	int i;
	int bitpos = 16;

	u32 pps_pic_parameter_set_id = h265_ue(buf, &bitpos);
	printf("pps_pic_parameter_set_id = %x\n", pps_pic_parameter_set_id);
	u32 pps_seq_parameter_set_id = h265_ue(buf, &bitpos);
	printf("pps_seq_parameter_set_id = %x\n", pps_seq_parameter_set_id);

	u8 dependent_slice_segments_enabled_flag = h265_u(buf, &bitpos,1);
	u8 output_flag_present_flag = h265_u(buf, &bitpos,1);
	u8 num_extra_slice_header_bits = h265_u(buf, &bitpos,3);
	printf("num_extra_slice_header_bits = %x\n", num_extra_slice_header_bits);
	u8 sign_data_hiding_enabled_flag = h265_u(buf, &bitpos,1);
	printf("sign_data_hiding_enabled_flag = %x\n",sign_data_hiding_enabled_flag);
	u8 cabac_init_present_flag = h265_u(buf, &bitpos,1);
	printf("cabac_init_present_flag = %x\n", cabac_init_present_flag);

	u32 num_ref_idx_l0_default_active_minus1 = h265_ue(buf, &bitpos);
	printf("num_ref_idx_l0_default_active_minus1 = %x\n", num_ref_idx_l0_default_active_minus1);
	u32 num_ref_idx_l1_default_active_minus1 = h265_ue(buf, &bitpos);
	printf("num_ref_idx_l1_default_active_minus1 = %x\n", num_ref_idx_l1_default_active_minus1);
	int init_qp_minus26 = h265_se(buf, &bitpos);
	printf("init_qp_minus26 = %x\n", init_qp_minus26);

	u8 constrained_intra_pred_flag = h265_u(buf, &bitpos,1);
	printf("constrained_intra_pred_flag = %x\n", constrained_intra_pred_flag);
	u8 transform_skip_enabled_flag = h265_u(buf, &bitpos,1);
	printf("transform_skip_enabled_flag = %x\n", transform_skip_enabled_flag);

	u8 cu_qp_delta_enabled_flag = h265_u(buf, &bitpos,1);
	printf("cu_qp_delta_enabled_flag = %x\n", cu_qp_delta_enabled_flag);
	if( cu_qp_delta_enabled_flag ){
		u32 diff_cu_qp_delta_depth = h265_ue(buf, &bitpos);
		printf("diff_cu_qp_delta_depth = %x\n",diff_cu_qp_delta_depth);
	}

	int pps_cb_qp_offset = h265_se(buf, &bitpos);
	printf("pps_cb_qp_offset = %d\n", pps_cb_qp_offset);

	int pps_cr_qp_offset = h265_se(buf, &bitpos);
	printf("pps_cr_qp_offset = %d\n", pps_cr_qp_offset);

	u8 pps_slice_chroma_qp_offsets_present_flag = h265_u(buf, &bitpos,1);
	printf("pps_slice_chroma_qp_offsets_present_flag = %x\n",pps_slice_chroma_qp_offsets_present_flag);
	u8 weighted_pred_flag = h265_u(buf, &bitpos,1);
	printf("weighted_pred_flag = %x\n",weighted_pred_flag);
	u8 weighted_bipred_flag = h265_u(buf, &bitpos,1);
	printf("weighted_bipred_flag = %x\n", weighted_bipred_flag);
	u8 transquant_bypass_enabled_flag = h265_u(buf, &bitpos,1);
	printf("transquant_bypass_enabled_flag = %x\n",transquant_bypass_enabled_flag);
	u8 tiles_enabled_flag = h265_u(buf, &bitpos,1);
	printf("tiles_enabled_flag = %x\n",tiles_enabled_flag);
	u8 entropy_coding_sync_enabled_flag = h265_u(buf, &bitpos,1);
	printf("entropy_coding_sync_enabled_flag = %x\n",entropy_coding_sync_enabled_flag);

	if( tiles_enabled_flag ) {
		u32 num_tile_columns_minus1 = h265_ue(buf, &bitpos);
		printf("num_tile_columns_minus1 = %x\n",num_tile_columns_minus1);
		u32 num_tile_rows_minus1 = h265_ue(buf, &bitpos);
		printf("num_tile_rows_minus1 = %x\n", num_tile_rows_minus1);

		u8 uniform_spacing_flag = h265_u(buf, &bitpos,1);
		u32 column_width_minus1[32];
		u32 row_height_minus1[32];
		if( !uniform_spacing_flag ) {
			for( i = 0; i < num_tile_columns_minus1; i++ ){
				column_width_minus1[i] = h265_ue(buf, &bitpos);
			}
			for( i = 0; i < num_tile_rows_minus1; i++ ){
				row_height_minus1[i] = h265_ue(buf, &bitpos);
			}
		}
		u8 loop_filter_across_tiles_enabled_flag = h265_u(buf, &bitpos,1);
	}

	u8 pps_loop_filter_across_slices_enabled_flag = h265_u(buf, &bitpos,1);
	printf("pps_loop_filter_across_slices_enabled_flag = %x\n",pps_loop_filter_across_slices_enabled_flag);
	u8 deblocking_filter_control_present_flag = h265_u(buf, &bitpos,1);
	printf("deblocking_filter_control_present_flag = %x\n",deblocking_filter_control_present_flag);
	if( deblocking_filter_control_present_flag ) {
		u8 deblocking_filter_override_enabled_flag = h265_u(buf, &bitpos,1);
		u8 pps_deblocking_filter_disabled_flag = h265_u(buf, &bitpos,1);
		if( !pps_deblocking_filter_disabled_flag ) {
			int pps_beta_offset_div2 = h265_se(buf, &bitpos);
			int pps_tc_offset_div2 = h265_se(buf, &bitpos);
		}
	}

	u8 pps_scaling_list_data_present_flag = h265_u(buf, &bitpos,1);
	printf("pps_scaling_list_data_present_flag = %x\n",pps_scaling_list_data_present_flag);
	if( pps_scaling_list_data_present_flag ){
		bitpos = scaling_list_data(buf, bitpos);
	}

	u8 lists_modification_present_flag = h265_u(buf, &bitpos,1);
	printf("lists_modification_present_flag = %x\n", lists_modification_present_flag);
	u32 log2_parallel_merge_level_minus2 = h265_ue(buf, &bitpos);
	printf("log2_parallel_merge_level_minus2 = %x\n", log2_parallel_merge_level_minus2);

	u8 slice_segment_header_extension_present_flag = h265_u(buf, &bitpos,1);
	printf("slice_segment_header_extension_present_flag = %x\n",slice_segment_header_extension_present_flag);

	u8 pps_extension_present_flag = h265_u(buf, &bitpos,1);
	printf("pps_extension_present_flag = %x\n",pps_extension_present_flag);
	if( pps_extension_present_flag ) {
		u8 pps_range_extension_flag = h265_u(buf, &bitpos,1);
		u8 pps_multilayer_extension_flag = h265_u(buf, &bitpos,1);
		u8 pps_3d_extension_flag = h265_u(buf, &bitpos,1);
		u8 pps_scc_extension_flag = h265_u(buf, &bitpos,1);
		u8 pps_extension_4bits = h265_u(buf, &bitpos,4);
	}
/*
	if( pps_range_extension_flag )
		pps_range_extension( )

	if( pps_multilayer_extension_flag )
		pps_multilayer_extension( )		// specified in Annex F

	if( pps_3d_extension_flag )
		pps_3d_extension( )		// specified in Annex I

	if( pps_scc_extension_flag )
		pps_scc_extension( )

	if( pps_extension_4bits )
		while( more_rbsp_data( ) )
			pps_extension_data_flag u(1)
*/
printf("}pps\n\n");
}




typedef struct {
	u8 uuid[16];
	u8 str[];
}sei5;
int parseh265_sei_user_data_unregistered(sei5* buf, int len)
{
	printf("uuid=\n");
	parseh265_printhex(buf->uuid, 16);

	printf("str=\n");
	printf("%.*s\n", len-16, buf->str);
	return 0;
}


typedef struct mastering_display_colour_volume{
	u16 display_primaries_x0;
	u16 display_primaries_y0;
	u16 display_primaries_x1;
	u16 display_primaries_y1;
	u16 display_primaries_x2;
	u16 display_primaries_y2;
	u16 white_point_x;
	u16 white_point_y;
	u32 max_display_mastering_luminance;
	u32 min_display_mastering_luminance;
}sei137;
int parseh265_sei_mastering_display_colour_volume(sei137* buf, int len)
{
	printf("display_primaries:(%d,%d),(%d,%d),(%d,%d)\n",
		parseh265_swap16(buf->display_primaries_x0),
		parseh265_swap16(buf->display_primaries_y0),
		parseh265_swap16(buf->display_primaries_x1),
		parseh265_swap16(buf->display_primaries_y1),
		parseh265_swap16(buf->display_primaries_x2),
		parseh265_swap16(buf->display_primaries_y2));
	printf("white_point:(%d,%d)\n",
		parseh265_swap16(buf->white_point_x),
		parseh265_swap16(buf->white_point_y));
	printf("mastering_lumance:(%d,%d)\n",
		parseh265_swap32(buf->min_display_mastering_luminance),
		parseh265_swap32(buf->max_display_mastering_luminance));
	return 0;
}


int parseh265_sei(unsigned char* buf, int len)
{
printf("sei{\n");
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
	case 0:
		//buffering_period( payloadSize )
		break;
	case 1:
		//pic_timing( payloadSize )
		break;
	case 2:
		//pan_scan_rect( payloadSize )
		break;
	case 3:
		//filler_payload( payloadSize )
		break;
	case 4:
		//user_data_registered_itu_t_t35( payloadSize )
		break;
	case 5:
		parseh265_sei_user_data_unregistered((void*)buf+old, size);
		break;
	case 6:
		//recovery_point( payloadSize )
		break;
	case 9:
		//scene_info( payloadSize )
		break;
	case 15:
		//picture_snapshot( payloadSize:
		break;
	case  16:
		//progressive_refinement_segment_start( payloadSize:
		break;
	case  17:
		//progressive_refinement_segment_end( payloadSize:
		break;
	case  19:
		//film_grain_characteristics( payloadSize:
		break;
	case  22:
		//post_filter_hint( payloadSize:
		break;
	case  23:
		//tone_mapping_info( payloadSize:
		break;
	case  45:
		//frame_packing_arrangement( payloadSize:
		break;
	case  47:
		//display_orientation( payloadSize:
		break;
	case  56:
		//green_metadata( payloadsize: /* specified in ISO/IEC 23001-11 */
		break;
	case  128:
		//structure_of_pictures_info( payloadSize:
		break;
	case  129:
		//active_parameter_sets( payloadSize:
		break;
	case  130:
		//decoding_unit_info( payloadSize:
		break;
	case  131:
		//temporal_sub_layer_zero_idx( payloadSize:
		break;
	case  133:
		//scalable_nesting( payloadSize:
		break;
	case  134:
		//region_refresh_info( payloadSize:
		break;
	case  135:
		//no_display( payloadSize:
		break;
	case  136:
		//time_code( payloadSize:
		break;
	case  137:
		parseh265_sei_mastering_display_colour_volume((void*)buf+old, size);
		break;
	case  138:
		//segmented_rect_frame_packing_arrangement( payloadSize:
		break;
	case  139:
		//temporal_motion_constrained_tile_sets( payloadSize:
		break;
	case  140:
		//chroma_resampling_filter_hint( payloadSize:
		break;
	case  141:
		//knee_function_info( payloadSize:
		break;
	case  142:
		//colour_remapping_info( payloadSize:
		break;
	case  143:
		//deinterlaced_field_identification( payloadSize:
		break;
	case  144:
		//content_light_level_info( payloadSize:
		break;
	case  145:
		//dependent_rap_indication( payloadSize:
		break;
	case  146:
		//coded_region_completion( payloadSize:
		break;
	case  147:
		//alternative_transfer_characteristics( payloadSize:
		break;
	case  148:
		//ambient_viewing_environment( payloadSize:
		break;
	case  149:
		//content_colour_volume( payloadSize:
		break;
	case  150:
		//equirectangular_projection( payloadSize:
		break;
	case  151:
		//cubemap_projection( payloadSize:
		break;
	case  152:
		//fisheye_video_info( payloadSize:
		break;
	case  154:
		//sphere_rotation( payloadSize:
		break;
	case  155:
		//regionwise_packing( payloadSize:
		break;
	case  156:
		//omni_viewport( payloadSize:
		break;
	case  157:
		//regional_nesting( payloadSize:
		break;
	case  158:
		//mcts_extraction_info_sets( payloadSize:
		break;
	case  159:
		//mcts_extraction_info_nesting( payloadSize:
		break;
	case  160:
		//layers_not_present( payloadSize: /* specified in Annex F */
		break;
	case  161:
		//inter_layer_constrained_tile_sets( payloadSize: /* specified in Annex F */
		break;
	case  162:
		//bsp_nesting( payloadSize: /* specified in Annex F */
		break;
	case  163:
		//bsp_initial_arrival_time( payloadSize: /* specified in Annex F */
		break;
	case  164:
		//sub_bitstream_property( payloadSize: /* specified in Annex F */
		break;
	case  165:
		//alpha_channel_info( payloadSize: /* specified in Annex F */
		break;
	case  166:
		//overlay_info( payloadSize: /* specified in Annex F */
		break;
	case  167:
		//temporal_mv_prediction_constraints( payloadSize: /* specified in Annex F */
		break;
	case  168:
		//frame_field_info( payloadSize: /* specified in Annex F */
		break;
	case  176:
		//three_dimensional_reference_displays_info( payloadSize: /* specified in Annex G */
		break;
	case  177:
		//depth_representation_info( payloadSize: /* specified in Annex G */
		break;
	case  178:
		//multiview_scene_info( payloadSize: /* specified in Annex G */
		break;
	case  179:
		//multiview_acquisition_info( payloadSize ) /* specified in Annex G */
		break;
	case 180:
		//multiview_view_position( payloadSize ) /* specified in Annex G */
		break;
	case 181:
		//alternative_depth_info( payloadSize ) /* specified in Annex I */
		break;
	case 200:
		//sei_manifest( payloadSize )
		break;
	case 201:
		//sei_prefix_indication( payloadSize )
		break;
	case 202:
		//annotated_regions( payloadSize )
		break;
	case 205:
		//shutter_interval_info( payloadSize )
		break;
	default:
		now = old + size;
		printf(".[%x,%x)payload\n",old,now);
		parseh265_printhex(buf+old, size);
	}
printf("}sei\n\n");
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
		parseh265_vps(buf, len);
		break;
	case 0x21:
		parseh265_sps(buf, len);
		break;
	case 0x22:
		parseh265_pps(buf, len);
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
		parseh265_sei(buf, len);
		break;
	case 0x28:
		printf("SEI_SUFFIX\n");
		break;
	}
	return 0;
}
