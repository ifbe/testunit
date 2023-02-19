#include <stdio.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;



IMFAttributes* attr = NULL;
IMFActivate** devices = NULL;
IMFMediaSource* source = NULL;
IMFSourceReader* reader = NULL;
//
IMFMediaType* mediatype = NULL;
GUID subtype;
//
u32 camcount = 0;
int exitflag;




void wstr2str(wchar_t* wstr,int wlen, char* str,int len){
	if(wlen>32)wlen = 32;
	int j;
	for(j=0;j<wlen;j++)str[j] = wstr[j];
	str[j] = 0;
}
void guid2str(GUID* guid, char* str){
	u8* buf = (u8*)guid;
	u8 c;
	for(int j=0;j<16;j++){
		c = (buf[j]&0xf) + '0';
		str[j*2+0] = (c<='9') ? c : c+7;
		c = (buf[j]>>4) + '0';
		str[j*2+1] = (c<='9') ? c : c+7;
	}
}
char* mf_subtype2format(GUID* guid, char* str){
	char const * c = 0;		//solve warning: ISO C++ forbids converting a string constant to 'char*'
	if(0 == guid){
		return 0;
	}

	//Uncompressed RGB Formats
	else if(IsEqualGUID(*guid, MFVideoFormat_RGB8)){
		c = "RGB8";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_RGB555)){
		c = "RGB555";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_RGB565)){
		c = "RGB565";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_RGB24)){
		c = "RGB24";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_RGB32)){
		c = "RGB32";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_ARGB32)){
		c = "ARGB32";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_A2R10G10B10)){
		c = "A2R10G10B10";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_A16B16G16R16F)){
		c = "A16B16G16R16F";
	}

	//YUV Formats: 8-Bit and Palettized
	else if(IsEqualGUID(*guid, MFVideoFormat_AI44)){
		c = "AI44";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_AYUV)){
		c = "AYUV";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_I420)){
		c = "I420";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_IYUV)){
		c = "IYUV";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_NV11)){
		c = "NV11";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_NV12)){
		c = "NV12";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_NV21)){
		c = "NV21";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_UYVY)){
		c = "UYVY";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y41P)){
		c = "Y41P";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y41T)){
		c = "Y41T";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y42T)){
		c = "Y42T";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_YUY2)){
		c = "YUV2";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_YVU9)){
		c = "YVU9";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_YV12)){
		c = "YV12";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_YVYU)){
		c = "YVYU";
	}

	//YUV Formats: 10-Bit and 16-Bit
	else if(IsEqualGUID(*guid, MFVideoFormat_P010)){
		c = "P010";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_P016)){
		c = "P016";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_P210)){
		c = "P210";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_P216)){
		c = "P216";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_v210)){
		c = "v210";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_v216)){
		c = "v216";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_v410)){
		c = "v410";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y210)){
		c = "Y210";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y216)){
		c = "Y216";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y410)){
		c = "Y410";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_Y416)){
		c = "Y416";
	}

	//Luminance and Depth Formats
	else if(IsEqualGUID(*guid, MFVideoFormat_L8)){
		c = "L8";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_L16)){
		c = "L16";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_D16)){
		c = "D16";
	}

	//Encoded Video Types
	else if(IsEqualGUID(*guid, MFVideoFormat_DV25)){
		c = "DV25";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DV50)){
		c = "DV50";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DVC)){
		c = "DVC";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DVH1)){
		c = "DVH1";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DVHD)){
		c = "DVHD";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DVSD)){
		c = "DVSD";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_DVSL)){
		c = "DVSL";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_H263)){
		c = "H263";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_H264)){
		c = "H264";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_H265)){
		c = "H265";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_H264_ES)){
		c = "H264_ES";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_HEVC)){
		c = "HEVC";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_HEVC_ES)){
		c = "HEVC_ES";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_M4S2)){
		c = "M4S2";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MJPG)){
		c = "MJPG";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MP43)){
		c = "MP43";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MP4S)){
		c = "MP4S";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MP4V)){
		c = "MP4V";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MPEG2)){
		c = "MPEG2";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_VP80)){
		c = "VP80";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_VP90)){
		c = "VP90";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MPG1)){
		c = "MPG1";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MSS1)){
		c = "MSS1";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_MSS2)){
		c = "MSS2";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_WMV1)){
		c = "WMV1";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_WMV2)){
		c = "WMV2";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_WMV3)){
		c = "WMV3";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_WVC1)){
		c = "WVC1";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_420O)){
		c = "420O";
	}
	else if(IsEqualGUID(*guid, MFVideoFormat_AV1)){
		c = "AV1";
	}

	if(c){
		return (char*)c;
	}
	else{
		guid2str(guid, str);
		return str;
	}
}
int mf_enum(IMFMediaSource*){
	printf("--------format enum--------:\n");
	IMFPresentationDescriptor* desc = 0;
	auto hr = source->CreatePresentationDescriptor(&desc);
	if(FAILED(hr)){
		printf("61\n");
		return 0;
	}
	if(0 == desc){
		printf("62\n");
		return 0;
	}

	DWORD desccount;
	hr = desc->GetStreamDescriptorCount(&desccount);
	if(FAILED(hr)){
		printf("63\n");
		return 0;
	}

	BOOL pfSelected;
	IMFStreamDescriptor* streamdesc;
	for(int j=0;j<desccount;j++){
		hr = desc->GetStreamDescriptorByIndex(j, &pfSelected, &streamdesc);

		wchar_t wstr[64];
		UINT32 wlen;
		streamdesc->GetString(MF_SD_STREAM_NAME, wstr, 64, &wlen);

		char str[64];
		wstr2str(wstr,wlen, str,64);
		printf("stream%d: %s\n", j, str);

		IMFMediaTypeHandler* hand;
		hr = streamdesc->GetMediaTypeHandler(&hand);

		DWORD cnt2;
		hr = hand->GetMediaTypeCount(&cnt2);
		for(int k=0;k<cnt2;k++){
			hr = hand->GetMediaTypeByIndex(k, &mediatype);
			hr = mediatype->GetGUID(MF_MT_SUBTYPE, &subtype);
			u32 width;
			u32 height;
			hr = MFGetAttributeSize(mediatype, MF_MT_FRAME_SIZE, &width, &height);
			u32 numerator;
			u32 denominator;
			hr = MFGetAttributeSize(mediatype, MF_MT_FRAME_RATE, &numerator, &denominator);

			char str[64];
			printf("%d: width=%d,height=%d,fps=%f,format=%.32s\n", k, width, height, (float)numerator/denominator, mf_subtype2format(&subtype, str));
		}
	}
	return 0;
}
int mf_select(IMFMediaSource*){
	printf("--------format select--------:\n");
	auto hr = MFCreateSourceReaderFromMediaSource(source, NULL, &reader);
	if(FAILED(hr)){
		printf("6\n");
	}

	hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediatype);
	if(FAILED(hr)){
		printf("7\n");
	}

	hr = mediatype->GetGUID(MF_MT_SUBTYPE, &subtype);
	if(FAILED(hr)){
		printf("8\n");
	}

	u32 width;
	u32 height;
	hr = MFGetAttributeSize(mediatype, MF_MT_FRAME_SIZE, &width, &height);
	if(FAILED(hr)){
		printf("9\n");
	}

	char str[64];
	printf("width=%d,height=%d,format=%.32s\n", width, height, mf_subtype2format(&subtype, str));

	return 0;
}
int main(){
	//Coinitialize(NULL);

	auto hr = MFStartup(MF_VERSION);
	if(FAILED(hr)){
		printf("0\n");
	}

	hr = MFCreateAttributes(&attr, 1);
	if(FAILED(hr)){
		printf("1\n");
	}

	hr = attr->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	if(FAILED(hr)){
		printf("2\n");
	}

	printf("--------device enum--------:\n");
	hr = MFEnumDeviceSources(attr, &devices, &camcount);
	if(FAILED(hr)){
		printf("3\n");
		return 0;
	}
	if(camcount < 1){
		printf("4\n");
		return 0;
	}
	for(int j=0;j<camcount;j++){
		wchar_t wstr[64];
		UINT32 wlen;
		devices[j]->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, wstr, 64, &wlen);

		char str[64];
		wstr2str(wstr,wlen, str,64);
		printf("%d: %s\n", j, str);
	}

	printf("--------device select--------:\n");
	hr = MFCreateDeviceSource(devices[0], &source);
	if(FAILED(hr)){
		printf("5\n");
	}
	//devices[0]->ActivateObject(IID_PPV_ARGS(source));

	mf_enum(source);

	mf_select(source);

	printf("--------run--------:\n");
	while(!exitflag){
		DWORD idx = 0;
		DWORD flag = 0;
		LONGLONG timestamp = 0;		//100ns
		IMFSample* sample;
		hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &idx, &flag, &timestamp, &sample);
		if(FAILED(hr)){
			printf("10\n");
		}
		if(0 == sample)continue;
		printf("sample:%d,%x,%lld,%p\n", idx,flag,timestamp/10/1000,sample);

		DWORD cnt = 0;
		DWORD len;
		u8* data;
		IMFMediaBuffer* buffer = NULL;
		hr = sample->GetBufferCount(&cnt);
		for(int i=0;i<cnt;i++){
			hr = sample->GetBufferByIndex(i, &buffer);
			if(0 == buffer)continue;

			hr = buffer->Lock(&data, NULL, &len);
			if(FAILED(hr)){
				printf("10\n");
			}

			char str[64];
			printf("type=%.32s,data=%p\n", mf_subtype2format(&subtype, str), data);

			hr = buffer->Unlock();
			if(FAILED(hr)){
				printf("11\n");
			}

			hr = buffer->Release();
			if(FAILED(hr)){
				printf("12\n");
			}
		}

		hr = sample->Release();
		if(FAILED(hr)){
			printf("13\n");
		}
	}
	attr->Release();
	source->Release();
	reader->Release();
	mediatype->Release();
	for(int i=0;i<camcount;i++){
		devices[i]->Release();
	}
	return 0;
}
