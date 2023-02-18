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
u32 width;
u32 height;
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
	if(IsEqualGUID(subtype, MFVideoFormat_AI44)){
		return "AI44";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_AYUV)){
		return "AYUV";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_I420)){
		return "I420";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_IYUV)){
		return "IYUV";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_NV11)){
		return "NV11";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_NV12)){
		return "NV12";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_NV21)){
		return "NV21";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_UYVY)){
		return "UYVY";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_Y41P)){
		return "Y41P";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_Y41T)){
		return "Y41T";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_Y42T)){
		return "Y42T";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_YUY2)){
		return "YUV2";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_YVU9)){
		return "YVU9";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_YV12)){
		return "YV12";
	}
	else if(IsEqualGUID(subtype, MFVideoFormat_YVYU)){
		return "YVYU";
	}
	else{
		guid2str(&subtype, str);
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

		IMFMediaTypeHandler* hand;
		hr = streamdesc->GetMediaTypeHandler(&hand);

		DWORD cnt2;
		hr = hand->GetMediaTypeCount(&cnt2);
		for(int k=0;k<cnt2;k++){
			hr = hand->GetMediaTypeByIndex(k, &mediatype);
			hr = MFGetAttributeSize(mediatype, MF_MT_FRAME_SIZE, &width, &height);
			hr = mediatype->GetGUID(MF_MT_SUBTYPE, &subtype);

			char str[64];
			printf("%d: width=%d,height=%d,format=%.32s\n", k, width, height, mf_subtype2format(&subtype, str));
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
