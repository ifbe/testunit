#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define hex32(a,b,c,d) (a|(b<<8)|(c<<16)|(d<<24))

/*
u8 g_sps_buf[] = {
0x67,0x64,0x00,0x15,0xac,0xd9,0x41,0xb1,
0xfe,0x4f,0x01,0x10,0x00,0x00,0x03,0x00,
0x10,0x00,0x00,0x03,0x03,0x20,0xf1,0x62,
0xd9,0x60,0x01};
u8 g_pps_buf[] = {
0x68,0xeb,0xec,0xb2,0x2c
};
*/


u16 swap16(u16 in)
{
	return ((in<<8)&0xff00) | ((in>>8)&0xff);
}
u32 swap32(u32 in)
{
	return (in>>24) | ((in>>8)&0xff00) | ((in<<8)&0xff0000) | (in<<24);
}
u64 swap64(u64 in)
{
	u64 lo = in&0xffffffff;
	u64 hi = (in>>32)&0xffffffff;
	lo = swap32(lo);
	hi = swap32(hi);
	return (lo<<32) | hi;
}
void print8(void* buf, int len)
{
	u8* p = buf;
	printf("%x,%x,%x,%x,%x,%x,%x,%x\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}


void getdatafromh264(u8* bs,int xx,
	int* spsoff,int* spsend,
	int* ppsoff,int* ppsend,
	int* idroff,int* idrend)
{
	int j;
	int flag = 0;
	for(j=3;j<0x10000;j++){
		if( (1==bs[j]) && (0==bs[j-1]) && (0==bs[j-2]) ){
			if(0x01 == (flag&3)){		//spsoff get, spsend not
				*spsend = (0==bs[j-3]) ? j-3 : j-2;
				flag |= 2;
			}
			if(0x00 == (flag&3)){		//spsoff not, spsend not
			if(7 == (bs[j+1]&0x1f)){
				*spsoff = j+1;
				flag |= 1;
			}
			}

			if(0x04 == (flag&0x0c)){	//ppsoff get, ppsend not
				*ppsend = (0==bs[j-3]) ? j-3 : j-2;
				flag |= 8;
			}
			if(0x00 == (flag&0x0c)){	//ppsoff not, ppsend not
			if(8 == (bs[j+1]&0x1f)){
				*ppsoff = j+1;
				flag |= 4;
			}
			}

			if(0x10 == (flag&0x30)){	//idroff get, idrend not
				*idrend = (0==bs[j-3]) ? j-3 : j-2;
				flag |= 0x20;
			}
			if(0x00 == (flag&0x30)){	//idroff not, idrend not
			if(5 == (bs[j+1]&0x1f)){
				*idroff = j+1;
				flag |= 0x10;
			}
			}

			if(0x3f==flag)return;	//all get
		}
	}
}
void parsenalu(u8* p, int l, int foff, int flen)
{
	int cnt = 0;
	int curr = 0, size = 0;
	do{
		size = (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | (p[3]);
		printf("		[%x,%x)nalu %d: %x,%x,%x,%x,%x,%x,%x,%x\n",
			foff+curr, foff+curr+size+4, cnt,
			p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]
		);
		p += size+4;
		curr += size+4;
		cnt++;
	}while(curr < l);
}


struct flvtag_video_h264_config{
	u8 version;
	u8 profile;
	u8 compatible;
	u8 level;
	u8 lenminus1;
	u8 raw[];

	//u8 numofsps;
	//struct flv_sps sps[];

	//u8 numofpps;
	//struct flv_pps pps[];
}__attribute__((packed));
struct flv_h264_sps{
	u16 len;
	u8 buf[];
}__attribute__((packed));
struct flv_h264_pps{
	u16 len;
	u8 buf[];
}__attribute__((packed));
void parsedeccfg(u8* p, int l, int foff, int flen)
{
/*
	printf("		[%x,%x)deccfg: %x,%x,%x,%x,%x,%x,%x,%x\n", foff, foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
*/
	struct flvtag_video_h264_config* cfg = (void*)p;
	int ver = cfg->version;
	int profile = cfg->profile;
	int compatible = cfg->compatible;
	int level = cfg->level;
	int len = (cfg->lenminus1&7) + 1;
	printf("		[%x,%x)ver=%x,profile=%x,compatible=%x,level=%x,len=%x\n",foff,foff+5,ver,profile,compatible,level,len);
	foff += 5;
	flen -= 5;

	int numofsps = cfg->raw[0]&0x1f;
	printf("		[%x,%x)numofsps=%x\n",foff,foff+1,numofsps);
	foff += 1;
	flen -= 1;

	int j,sz;
	struct flv_h264_sps* sps = (void*)(cfg->raw + 1);
	for(j=0;j<numofsps;j++){
		sz = swap16(sps->len);
		printf("			[%x,%x)id=%d,sz=%x\n",foff,foff+2, j, sz);
		printf("			[%x,%x): ",foff+2,foff+2+sz);
		print8(sps->buf, sz);

		foff += 2+sz;
		flen -= 2+sz;
		sps = (void*)sps + 2 + sz;
	}

	int numofpps = *(u8*)sps;
	printf("		[%x,%x)numofpps=%x\n",foff,foff+1,numofpps);
	foff += 1;
	flen -= 1;

	struct flv_h264_pps* pps = (void*)sps + 1;
	for(j=0;j<numofpps;j++){
		sz = swap16(pps->len);
		printf("			[%x,%x)id=%d,sz=%x\n",foff,foff+2, j, sz);
		printf("			[%x,%x): ",foff+2,foff+2+sz);
		print8(pps->buf, sz);

		foff += 2+sz;
		flen -= 2+sz;
		pps = (void*)pps + 2 + sz;
	}

}
int makedeccfg(u8* p, int xxx, u8* spsbuf,int spslen, u8* ppsbuf, int ppslen)
{
	int cnt = 0;

	//head
	struct flvtag_video_h264_config* cfg = (void*)p;
	cfg->version = 1;
	cfg->profile = 0x64;
	cfg->compatible = 0;
	cfg->level = 0x15;
	cfg->lenminus1 = 0xf8 | (8-1);
	cnt = 5;

	//num of sps
	p[cnt] = 1;
	cnt += 1;

	//one sps
	struct flv_h264_sps* sps = (void*)p+cnt;
	sps->len = swap16(spslen);
	memcpy(sps->buf, spsbuf, spslen);
	cnt += 2+spslen;

	//num of pps
	p[cnt] = 1;
	cnt += 1;

	//one pps
	struct flv_h264_pps* pps = (void*)p+cnt;
	pps->len = swap16(ppslen);
	memcpy(pps->buf, ppsbuf, ppslen);
	cnt += 2+ppslen;

	return cnt;
}


struct flvtag_video{
	u8 frametype:4;
	u8 codecid:4;
}__attribute__((packed));
struct flvtag_video_h264{
	u8 codecid:4;	//lo4
	u8 frametype:4;	//hi4
	u8 avc_type;
	u8 avc_cts[3];
	u8 avc_data[];
}__attribute__((packed));
void parsevideo(u8* p, int l, int foff, int flen)
{
	//printf("%x,%x,%x,%x,%x,%x,%x,%x\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

	struct flvtag_video_h264* tagvideo = (void*)p;
	u8 frametype = tagvideo->frametype;
	u8 codecid = tagvideo->codecid;

	u8 avc_type;
	int avc_dt;
	struct flvtag_video_h264* tagh264 = (void*)p;
	switch(codecid){
	case 7:
		avc_type = tagh264->avc_type;
		avc_dt = ((int)tagh264->avc_cts[0]<<16) | (tagh264->avc_cts[1]<<8) | (tagh264->avc_cts[2]);
		printf("	[%x,%x)h264: frametype=%x,codecid=%x,avc_type=%x,avc_dt=%d: ",
			foff, foff+flen,
			frametype, codecid,
			avc_type, avc_dt
		);
		print8(p, 8);
		switch(avc_type){
		case 0:
			parsedeccfg(tagh264->avc_data, flen-5, foff+5, flen-5);
			break;
		case 1:
			parsenalu(tagh264->avc_data, flen-5, foff+5, flen-5);
			break;
		case 2:
			printf("		end of avc: ");
			print8(tagh264->avc_data, flen-5);
			break;
		default:
			print8(tagh264->avc_data, flen-5);
			break;
		}
		break;
	case 12:
		printf("	[%x,%x)h265: frametype=%x,codecid=%x: ",foff,foff+flen,frametype, codecid);
		print8(p, 8);
	default:
		printf("	[%x,%x)video: frametype=%x,codecid=%x: ",foff,foff+flen,frametype, codecid);
		print8(p, 8);
		break;
	}
}
int makeh264_deccfg(u8* buf, int len, u8* spsbuf, int spslen, u8* ppsbuf, int ppslen)
{
	int cnt = 0;

	struct flvtag_video_h264* video = (void*)buf;
	video->frametype = 1;
	video->codecid = 7;
	video->avc_type = 0;
	video->avc_cts[0] = video->avc_cts[1] = video->avc_cts[2] = 0;
	cnt += 5;

	cnt += makedeccfg(video->avc_data,0, spsbuf,spslen, ppsbuf,ppslen);
	return cnt;
}
int makeh264_bs(u8* buf, int len, u8* nalubuf, int nalulen)
{
	int cnt = 0;

	struct flvtag_video_h264* video = (void*)buf;
	video->frametype = 1;
	video->codecid = 7;
	video->avc_type = 1;
	video->avc_cts[0] = video->avc_cts[1] = video->avc_cts[2] = 0;
	cnt += 5;

	u8* p = video->avc_data;
	p[0] = (nalulen>>24)&0xff;
	p[1] = (nalulen>>16)&0xff;
	p[2] = (nalulen>>8)&0xff;
	p[3] = nalulen&0xff;
	cnt += 4;

	memcpy(video->avc_data+4, nalubuf, nalulen);
	cnt += nalulen;

	return cnt;
}


void parseaudio(u8* p, int l, int foff, int flen)
{
	printf("	[%x,%x)audio: %x,%x,%x,%x,%x,%x,%x,%x\n",
		foff,foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
}
int makepcm_s16le_44k1ch(u8* p)
{
#define AUDIO_TYPE_PCM 3
#define AUDIO_FREQ_44100 3
#define AUDIO_SIZE_16BIT 1
#define AUDIO_CHAN_MONO 0
#define stage 1.0594630943592953
	p[0] = (AUDIO_TYPE_PCM<<4) | (AUDIO_FREQ_44100<<2) | (AUDIO_SIZE_16BIT<<1) | (AUDIO_CHAN_MONO);

	int j,k;
	float freq = 440;
	short* pcm = (void*)(p+1);
	for(k=0;k<12;k++){
		printf("freq=%f\n",freq);
		for(j=0;j<4410;j++){
			pcm[k*44100/12 + j] = (short)(sin(j*3.141592653*2*freq/44100)*32767*(1.0-j/4410.0));
		}
		freq *= stage;
	}
	return 1+44100*2;
}


struct flvtag_script_onMetaData{
	u8 onMetaData_0002;
	u8 onMetaData_size[2];
	u8 onMetaData_str[0];
}__attribute__((packed));
struct flvtag_script_inner{
	u8 type;
	u32 count;
	u8 each[];
}__attribute__((packed));
struct flvtag_script_each{
	u16 size;
	u8 str[];
}__attribute__((packed));
struct flvtag_script_eachdata{
	u8 type;	//0=double,1=u8,2=str
	union{
		double _d64;
		u64 _u64;
		u8 _u8;
		struct str{
			u16 len;
			u8* ptr;
		}str;
	};
	//u8 datadata[]
}__attribute__((packed));
int makescript_each(u8* buf, int len, u8* str, double val)
{
	int sl = strlen((char*)str);

	struct flvtag_script_each* eachstr = (void*)buf;
	eachstr->size = swap16(sl);
	strncpy((char*)eachstr->str, (char*)str, sl);

	struct flvtag_script_eachdata* eachdata = (void*)(buf+2+sl);
	eachdata->type = 0;
	eachdata->_u64 = swap64(*(u64*)&val);
	return 2+sl+1+sizeof(double);
}
int makescript(u8* buf, int len)
{
	int cnt = 0;

	//02 00 0a
	struct flvtag_script_onMetaData* ptr = (void*)buf;
	ptr->onMetaData_0002 = 2;
	ptr->onMetaData_size[0] = 0;ptr->onMetaData_size[1] = 0xa;
	strncpy((char*)ptr->onMetaData_str, "onMetaData", 10);
	cnt += 1+2+10;

	//08 00 00 00 xx
	struct flvtag_script_inner* inner = (void*)(buf+cnt);
	inner->type = 8;
	inner->count = swap32(4);
	cnt += 1+4;

	//width = 1920.0
	cnt += makescript_each(buf+cnt, 0x1000, (u8*)"duration", 1.0);	//make ffprobe happy
	cnt += makescript_each(buf+cnt, 0x1000, (u8*)"width", 1920.0);
	cnt += makescript_each(buf+cnt, 0x1000, (u8*)"height", 1080.0);
	cnt += makescript_each(buf+cnt, 0x1000, (u8*)"videocodecid", 7.0);

	//0,0,9
	buf[cnt+0] = 0;
	buf[cnt+1] = 0;
	buf[cnt+2] = 9;
	cnt += 3;

	return cnt;
}
void parsescript(u8* p, int l, int foff, int flen)
{
/*
	printf("	[%x,%x)script: %x,%x,%x,%x,%x,%x,%x,%x\n",
		foff,foff+flen,
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
*/
	struct flvtag_script_onMetaData* ptr = (void*)p;
	int type = ptr->onMetaData_0002;
	int size = (ptr->onMetaData_size[0]<<8) | ptr->onMetaData_size[1];
	printf("	[%x,%x)AMF0: 0002=%x,len=%x,str='%.*s'\n",foff,foff+3+size,type,size,size,ptr->onMetaData_str);
	foff += 3+size;
	flen -= 3+size;

	struct flvtag_script_inner* inner = (void*)(ptr->onMetaData_str + size);
	u32 count = swap32(inner->count);
	printf("	[%x,%x)AMF1: type=%x,count=%x\n",foff,foff+flen, inner->type, count);
	foff += 5;
	flen -= 5;

	int j;
	int datatype;
	int size1,size2;
	u64 temp;
	struct flvtag_script_each* each = (void*)inner->each;
	for(j=0;j<count;j++){
		//size1=strlen, size2=unionlen, size=eachlen
		size1 = swap16(each->size);
		datatype = each->str[size1];
		switch(datatype){
		case 2:
			size2 = 2 + (each->str[size1+1]<<8) + (each->str[size1+2]);
			size = (2+size1) + (1+size2);
			printf("		[%x,%x)%x: %.*s = %.*s\n", foff,foff+size,j,
				size1, each->str,
				size2-2, each->str+size1+1+2
			);
			print8(each->str+size1+1+2, size2-2);
			break;
		case 1:
			size2 = 1;
			size = (2+size1) + (1+size2);
			printf("		[%x,%x)%x: %.*s = %x\n", foff,foff+size,j,
				size1,each->str,
				each->str[size1+1]
			);
			break;
		case 0:
		default:
			size2 = 8;
			size = (2+size1) + (1+size2);
			temp = swap64(*(u64*)(each->str+size1+1));
			printf("		[%x,%x)%x: %.*s = %lf\n", foff,foff+size,j,
				size1,each->str,
				*(double*)&temp
			);
		}
		//printf("size1=%x,size2=%x\n",size1,size2);
		foff += size;
		flen -= size;
		each = ((void*)each) + size;
	}

	u8* end = (void*)each;
	printf("	[%x,%x)end: %x,%x,%x\n",foff, foff+flen, end[0],end[1],end[2]);
}


struct flvhead{
	u8 flv[3];
	u8 ver;
	u8 attr;
	u32 size;	//0,0,0,9
	//u8 prevsize[4];
}__attribute__((packed));
int flv_maker_head(struct flvhead* head){
	head->flv[0] = 'F';
	head->flv[1] = 'L';
	head->flv[2] = 'V';
	head->ver = 1;
	head->attr = 1;		//1=video, 0x4=audio, 0x5=audio|video
	head->size = swap32(9);
	return 9;
}
struct flvtag{
	u8 type;
	u8 thissize[3];
	u8 time[3];
	u8 time_ext;
	u8 stream[3];
	u8 data[0];
	//u8 prevsize[4];
}__attribute__((packed));
int flv_maker_script(struct flvtag* tag){
	int len = makescript((void*)tag->data, 0x1000);
	tag->type = 0x12;
	tag->thissize[0] = (len>>16)&0xff;
	tag->thissize[1] = (len>>8)&0xff;
	tag->thissize[2] = len&0xff;
	tag->time[0] = tag->time[1] = tag->time[2] = 0;
	tag->time_ext = 0;
	tag->stream[0] = tag->stream[1] = tag->stream[2] = 0;
	return len+11;
}
int flv_maker_h264_deccfg(struct flvtag* tag,int xxx, u8* spsbuf,int spslen, u8* ppsbuf,int ppslen){
	int len = makeh264_deccfg((void*)tag->data, 0x1000, spsbuf,spslen, ppsbuf,ppslen);
	tag->type = 0x9;
	tag->thissize[0] = (len>>16)&0xff;
	tag->thissize[1] = (len>>8)&0xff;
	tag->thissize[2] = len&0xff;
	tag->time[0] = tag->time[1] = tag->time[2] = 0;
	tag->time_ext = 0;
	tag->stream[0] = tag->stream[1] = tag->stream[2] = 0;
	return len+11;
}
int flv_maker_h264_bs(struct flvtag* tag, int xxx, u8* nalubuf, int nalulen){
	int len = makeh264_bs((void*)tag->data, 0x1000, nalubuf, nalulen);
	tag->type = 0x9;
	tag->thissize[0] = (len>>16)&0xff;
	tag->thissize[1] = (len>>8)&0xff;
	tag->thissize[2] = len&0xff;
	tag->time[0] = tag->time[1] = tag->time[2] = 0;
	tag->time_ext = 0;
	tag->stream[0] = tag->stream[1] = tag->stream[2] = 0;
	return len+11;
}
int flv_maker_pcm_s16le_44k1ch(struct flvtag* tag){
	int len = makepcm_s16le_44k1ch((void*)tag->data);
	tag->type = 0x8;
	tag->thissize[0] = (len>>16)&0xff;
	tag->thissize[1] = (len>>8)&0xff;
	tag->thissize[2] = len&0xff;
	tag->time[0] = tag->time[1] = tag->time[2] = 0;
	tag->time_ext = 0;
	tag->stream[0] = tag->stream[1] = tag->stream[2] = 0;
	return len+11;
}




int flv_parser(char* filename)
{
	unsigned char tmp[0x100000];

	FILE* fp = fopen(filename,"rb");
	if(!fp)return 0;

	int ret = fread(tmp, 1, 0x100, fp);
	if(ret < 9)return 0;

	struct flvhead* head = (void*)tmp;
	int hlen = swap32(head->size);
	printf("[%x,%x): ver=%x,attr=%x,size=%x\n", 0,9, head->ver, head->attr, hlen);
	printf("[%x,%x): prevsize=%x\n\n", 9,0xd, swap32( *(u32*)(tmp+hlen)) );

	int j = hlen+4, k = 0;

	u32 type,thissize,time,stream;
	struct flvtag* tag = (void*)tmp;

	int prevsize;
	u8* ptr;
	while(1){
		ret = fseek(fp, j, SEEK_SET);
		ret = fread(tmp, 1, 0x100000, fp);
		if(ret < 11)break;

		type = tag->type;
		thissize = (tag->thissize[0]<<16) | (tag->thissize[1]<<8) | tag->thissize[2];
		time = (tag->time[0]<<16) | (tag->time[1]<<8) | tag->time[2];
		stream = (tag->stream[0]<<16) | (tag->stream[1]<<8) | tag->stream[2];

		k = j+11+thissize;
		printf("[%x,%x): type=%x,thissize=%x,time=%.3f,stream=%x\n", j,k, type,thissize,(float)time*0.001,stream);
		switch(type){
		case 9:
			parsevideo(tag->data, thissize, j+11, k-j-11);
			break;
		case 8:
			parseaudio(tag->data, thissize, j+11, k-j-11);
			break;
		case 18:
			parsescript(tag->data, thissize, j+11, k-j-11);
			break;
		}

		ptr = tmp+11+thissize;
		prevsize = (ptr[0]<<24) | (ptr[1]<<16) | (ptr[2]<<8) | ptr[3];
		printf("[%x,%x): prevsize=%x(b+%x)\n\n",k,k+4,prevsize,prevsize-11);

		j = k+4;	//prevsize at the end
	}

	fclose(fp);
	return 0;
}
int flv_maker(char* flvname, char* h264name)
{
	unsigned char bs[0x10000];
	FILE* fp = fopen(h264name,"rb");
	printf("fp=%p\n",fp);
	u32 rlen = fread(bs, 1, 0x10000, fp);
	printf("rlen=%x\n",rlen);
	fclose(fp);

	int spsoff=-1,spsend=-1;
	int ppsoff=-1,ppsend=-1;
	int idroff=-1,idrend=-1;
	getdatafromh264(bs, 0x10000, &spsoff,&spsend, &ppsoff,&ppsend, &idroff,&idrend);
	printf("[%x,%x)sps\n",spsoff,spsend);
	printf("[%x,%x)pps\n",ppsoff,ppsend);
	printf("[%x,%x)idr\n",idroff,idrend);
	if((spsoff<0)|(spsend<0))return 0;
	if((ppsoff<0)|(ppsend<0))return 0;
	if((idroff<0)|(idrend<0))return 0;
	printf("----h264 ok----\n\n");

	int offs = 0;
	unsigned char tmp[0x10000];
	FILE* fo = fopen(flvname, "wb");


	//000000000
	struct flvhead* head = (void*)tmp;
	int len = flv_maker_head(head);
	int ret = fwrite(head, 1, len, fo);
	printf("[0,9)ret=%x\n",ret);
	offs = 9;

	u32* prevsize = (void*)tmp+9;
	ret = fwrite(prevsize, 1, 4, fo);
	printf("[9,d)ret=%x\n",ret);
	offs = 9+4;


	//111111111
	struct flvtag* tagscript = (void*)tmp+0xd;
	len = flv_maker_script(tagscript);
	ret = fwrite(tagscript, 1, len, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+len, ret);
	offs += len;

	prevsize = (void*)tagscript+len;
	*prevsize = swap32(len);
	ret = fwrite(prevsize, 1, 4, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+4, ret);
	offs += 4;


	//222222222
	struct flvtag* tagavchead = (void*)tmp+offs;
	len = flv_maker_h264_deccfg(tagavchead,0, bs+spsoff,spsend-spsoff, bs+ppsoff,ppsend-ppsoff );
	ret = fwrite(tagavchead, 1, len, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+len, ret);
	offs += len;

	prevsize = (void*)tagavchead+len;
	*prevsize = swap32(len);
	ret = fwrite(prevsize, 1, 4, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+4, ret);
	offs += 4;


	//3333333333
	struct flvtag* tagavcbs = (void*)tmp+offs;
	len = flv_maker_h264_bs(tagavcbs, 0, bs+idroff, idrend-idroff);
	ret = fwrite(tagavcbs, 1, len, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+len, ret);
	offs += len;

	prevsize = (void*)tagavcbs+len;
	*prevsize = swap32(len);
	ret = fwrite(prevsize, 1, 4, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+4, ret);
	offs += 4;


	//44444444
	struct flvtag* tagpcm = (void*)tmp+offs;
	len = flv_maker_pcm_s16le_44k1ch(tagpcm);
	ret = fwrite(tagpcm, 1, len, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+len, ret);
	offs += len;

	prevsize = (void*)tagpcm+len;
	*prevsize = swap32(len);
	ret = fwrite(prevsize, 1, 4, fo);
	printf("[%x,%x)ret=%x\n", offs, offs+4, ret);
	offs += 4;


	fclose(fo);
	return 0;
}
int main(int argc, char** argv)
{
	if(argc < 3)goto help;
	if(0 == strncmp(argv[1], "make", 4)){
		return flv_maker(argv[2], argv[3]);
	}
	if(0 == strncmp(argv[1], "parse", 5)){
		return flv_parser(argv[2]);
	}
	//fallthrough
help:
	printf("./a.out parse /path/test.flv\n");
	printf("./a.out make /path/test.flv /path/bs.h264\n");
	return 0;
}
