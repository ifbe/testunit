#include <windows.h>
#include <dshow.h>
#include <rpc.h>
#include <rpcndr.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#pragma comment(lib, "ole32")
#pragma comment(lib, "strmiids")
EXTERN_C const CLSID CLSID_NullRenderer;
EXTERN_C const CLSID CLSID_SampleGrabber;

#ifndef SAMPLE_GRABBER_H
#define SAMPLE_GRABBER_H
DEFINE_GUID(IID_ISampleGrabberCB, 0x0579154A, 0x2B53, 0x4994, 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85);
MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime,IMediaSample* pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime,BYTE *pBuffer,long BufferLen) = 0;
};

DEFINE_GUID(IID_ISampleGrabber, 0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);
MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(BOOL OneShot) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMediaType(const AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(BOOL BufferThem) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(long *pBufferSize,long *pBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(IMediaSample **ppSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCallback(ISampleGrabberCB *pCallback,long WhichMethodToCallback) = 0;
};
#endif




class CallbackObject : public ISampleGrabberCB
{
public:
	CallbackObject() {};
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		if (NULL == ppv) return E_POINTER;
		printf("%llx\n",riid);
		if (riid == __uuidof(IUnknown))
		{
			*ppv = static_cast<IUnknown*>(this);
			return S_OK;
		}
		//if (riid == __uuidof(ISampleGrabberCB))   {
		else
		{
			*ppv = static_cast<ISampleGrabberCB*>(this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHODIMP_(ULONG) AddRef() {return S_OK;}
	STDMETHODIMP_(ULONG) Release(){return S_OK;}

	//ISampleGrabberCB
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
	STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen){return S_OK;}
};
STDMETHODIMP CallbackObject::SampleCB(double SampleTime, IMediaSample *pSample)
{
	int s;
	BYTE* p = NULL;

	s = pSample->GetActualDataLength();
	pSample->GetPointer(&p);
	pSample->Release();

	printf("%llx,%x\n", p, s);
	printf("%x %x %x %x\n",p[0],p[1],p[2],p[3]);

	pSample->Release();
	return S_OK;
}




HRESULT enumdev(void* dev, int which)
{
	//enumerator
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
	IPropertyBag *pPropBag;
	VARIANT var;
	HRESULT hr;
	int flag = -1;

	//Create the System Device Enumerator.
	hr = CoCreateInstance(
		CLSID_SystemDeviceEnum, NULL,  
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum)
	);
	if(FAILED(hr)){printf("error@ICreateDevEnum\n");return -1;}

	//Create an enumerator for the category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
	if (hr == S_FALSE){printf("error@IEnumMoniker\n");return -2;}

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;  
		} 

		var.vt = VT_BSTR;
/*
		hr = pPropBag->Read(L"devPath", &var, 0);
		if (SUCCEEDED(hr))
		{
			// The dev path is not intended for display.
			printf("dev path: %S\n", var.bstrVal);
		}
*/

		// Get description or friendly name.
		hr = pPropBag->Read(L"Description", &var, 0);
		if (FAILED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &var, 0);
		}
		if (SUCCEEDED(hr))
		{
			if(which == 0)
			{
				hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)dev);
				if(FAILED(hr))printf("error@bindtoobject\n");
				else
				{
					printf("%S	**selected**\n", var.bstrVal);
					flag = 1;
				}
			}
			else printf("%S\n", var.bstrVal);

			which--;
		}

		pPropBag->Release();
		pMoniker->Release();
	}

	//
	pEnum->Release();
	pDevEnum->Release();
	return flag;
}
HRESULT enumpin(IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    IEnumPins* pEnum;
    *ppPin = NULL;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;  // Return the pin's interface
                hr = S_OK;      // Found requested pin, so clear error
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}
HRESULT enumfmt(IPin* pin)
{
	HRESULT hr;
	IEnumMediaTypes *mtEnum=NULL;  
    AM_MEDIA_TYPE   *mt=NULL;  
    if( FAILED( pin->EnumMediaTypes( &mtEnum )) )return -1;
    mtEnum->Reset();

	VIDEOINFOHEADER* head;
	BITMAPINFOHEADER* bmp;
    ULONG mtFetched = 0;
    while (SUCCEEDED(mtEnum->Next(1, &mt, &mtFetched)) && mtFetched)
	{
		if(mt->majortype == MEDIATYPE_Video)
		{
			//printf("video,");
			if(mt->subtype == MEDIASUBTYPE_YUY2)
			{
				//printf("yuyv,");
				if(mt->formattype == FORMAT_VideoInfo)
				{
					head = (VIDEOINFOHEADER*)(mt->pbFormat);
					bmp = &(head->bmiHeader);
					//printf("%dx%d", bmp->biWidth, bmp->biHeight);
				}
				//else printf("???");
			}
			//else printf("???");
		}
		//else printf("???");

		//printf("\n");
	}

	return 1;
}
HRESULT configgraph(IAMStreamConfig* devcfg)
{
	int iCount = 0;
	int iSize = 0;
	HRESULT hr;

	VIDEO_STREAM_CONFIG_CAPS scc;
	AM_MEDIA_TYPE *pmtConfig;
	VIDEOINFOHEADER* head;
	BITMAPINFOHEADER* bmp;

	hr = devcfg->GetNumberOfCapabilities(&iCount, &iSize);
	if(FAILED(hr)){printf("%x@GetNumberOfCapabilities\n",hr);return -2;}

	// Check the size to make sure we pass in the correct structure.
	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		// Use the video capabilities structure.
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			hr = devcfg->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
			if (SUCCEEDED(hr))
			{
				head = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
				bmp = &(head->bmiHeader);
				printf("%dx%d\n", bmp->biWidth, bmp->biHeight);

				if( (bmp->biWidth== 640) && (bmp->biHeight==480) )
				{
					hr = devcfg->SetFormat(pmtConfig);
					if(hr == S_OK)printf("success\n");
					else printf("error %x\n", hr);
				}
				//DeleteMediaType(pmtConfig);
			}
		}
	}
/*
	if( (bmp->biWidth==640) && (bmp->biHeight==480) )
	{
		if(devcfg != 0)
		{
			hr = devcfg->SetFormat(mt);
			if(hr == S_OK)printf("	***selected***");
			else printf("	***%x***",hr);
		}
	}
*/
	return S_OK;
}
void letsgo()
{
	//builder
	HRESULT hr;
	IGraphBuilder* m_pGraph;
	ICaptureGraphBuilder2* m_pBuild;
	IMediaControl * g_pMC = NULL;
	IMediaEventEx * g_pME = NULL;
	IVideoWindow  * g_pVW = NULL;

	//src
	IBaseFilter* dev = NULL;
	IPin* devout = 0;
	IAMStreamConfig *devcfg = NULL;

	//dst
	IBaseFilter* sample = NULL;
	IPin* samplein = 0;
	ISampleGrabber* pGrabber = NULL;
	CallbackObject cb;

	AM_MEDIA_TYPE* mt;
	VIDEOINFOHEADER* head;
	BITMAPINFOHEADER* bmp;




	//com init
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hr))goto end;

	//builder
	hr = CoCreateInstance(
		CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
		(void**)&m_pBuild
	);
	if(FAILED(hr)){printf("error %x@CLSID_CaptureGraphBuilder2\n",hr);goto fail;}

	hr = CoCreateInstance(
		CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
		(void**)&m_pGraph
	);
	if(FAILED(hr)){printf("error %x@CLSID_FilterGraph\n",hr);goto fail;}

	hr = m_pBuild->SetFiltergraph(m_pGraph);
	if(FAILED(hr)){printf("error %x@SetFiltergraph\n",hr);goto fail;}

	hr = m_pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &g_pMC);
    if (FAILED(hr)){printf("mediacontrol\n");return;}

    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &g_pVW);
    if (FAILED(hr)){printf("videowindow\n");return;}

    hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *) &g_pME);
    if (FAILED(hr)){printf("mediaevent\n");return;}




	//dev filter
	hr = enumdev(&dev, 1);
	if(hr < 0)goto fail;

	hr = m_pGraph->AddFilter(dev, L"Capture Filter");
	if(FAILED(hr)){printf("error %x@add dev\n",hr);goto fail;}

	hr = enumpin(dev, PINDIR_OUTPUT, 0, &devout);
	if(hr < 0)goto fail;

	hr = enumfmt(devout);
	if(hr < 0)goto fail;




	//sample filter
	hr = CoCreateInstance(
		CLSID_SampleGrabber, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter,
		(void**)&sample
	);
	if(FAILED(hr)){printf("error %x@sample create\n",hr);goto fail;}

	hr = sample->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	if(FAILED(hr)){printf("error %x@sample query\n",hr);goto fail;}

	hr = pGrabber->SetOneShot(FALSE);
	hr = pGrabber->SetBufferSamples(TRUE);

	hr = pGrabber->SetCallback(&cb, 0);
	if(FAILED(hr)){printf("error %x@sample callback\n",hr);goto fail;}

	hr = m_pGraph->AddFilter(sample, L"Sample Grabber");
	if(FAILED(hr)){printf("error %x@add sample\n",hr);goto fail;}

	hr = enumpin(sample, PINDIR_INPUT, 0, &samplein);

	hr = enumfmt(samplein);




	//connect, start
	hr = m_pBuild->FindInterface(
		&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		dev, IID_IAMStreamConfig,
		(void**)&devcfg
	);
	if(FAILED(hr)){printf("%x@findinterface\n",hr);goto fail;}

	hr = configgraph(devcfg);

    hr = m_pGraph->Connect(devout, samplein);
	if(FAILED(hr)){printf("error %x@graph connect\n",hr);goto fail;}
/*
	hr = pGrabber->GetConnectedMediaType(&mt);
	if(FAILED(hr)){printf("error %x@media type\n",hr);goto fail;}
*/
	hr = devcfg->GetFormat(&mt);
	head = (VIDEOINFOHEADER*)(mt->pbFormat);
	bmp = &(head->bmiHeader);
	printf("[mediatype]%dx%d\n", bmp->biWidth, bmp->biHeight);

	hr = g_pMC->Run();
	if(FAILED(hr)){printf("error %x@run\n",hr);goto fail;}

	return;




fail:
	//com uninit
	CoUninitialize();
end:
	return;
}




int main()
{
	letsgo();
	while(1){Sleep(1000);}
}
