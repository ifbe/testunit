#include <windows.h>
#include <dshow.h>
#include <rpc.h>
#include <rpcndr.h>
#pragma comment(lib, "ole32")
#pragma comment(lib, "strmiids")
EXTERN_C const CLSID CLSID_NullRenderer;
EXTERN_C const CLSID CLSID_SampleGrabber;

#ifndef SAMPLE_GRABBER_H
#define SAMPLE_GRABBER_H
DEFINE_GUID(IID_ISampleGrabberCB,0x0579154A, 0x2B53, 0x4994, 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85);
MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime,IMediaSample* pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime,BYTE *pBuffer,long BufferLen) = 0;
};

DEFINE_GUID(IID_ISampleGrabber,0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);
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
	BYTE *p = NULL;
	pSample->GetPointer(&p);
	printf("%x %x %x %x\n",p[0],p[1],p[2],p[3]);
	return S_OK;

	if (!pSample)return E_POINTER;
	long sz = pSample->GetActualDataLength();
	pSample->GetPointer(&p);
	if (sz <= 0 || p==NULL) return E_UNEXPECTED;
	pSample->Release();
	return S_OK;
}



HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    IEnumPins* pEnum;
    *ppPin = NULL;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

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
IPin* GetInPin( IBaseFilter * pFilter, int nPin )
{
    IPin* pComPin=0;
    GetPin(pFilter, PINDIR_INPUT, nPin, &pComPin);
    return pComPin;
}
IPin* GetOutPin( IBaseFilter * pFilter, int nPin )
{
    IPin* pComPin=0;
    GetPin(pFilter, PINDIR_OUTPUT, nPin, &pComPin);
    return pComPin;
}




int choose(void* device, int which)
{
	//enumerator
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
	IPropertyBag *pPropBag;
	VARIANT var;
	HRESULT hr;

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
		hr = pPropBag->Read(L"DevicePath", &var, 0);
		if (SUCCEEDED(hr))
		{
			// The device path is not intended for display.
			printf("Device path: %S\n", var.bstrVal);
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
				hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)device);
				if(FAILED(hr))printf("error@bindtoobject\n");
				else printf("%S	**selected**\n", var.bstrVal);
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
	return 1;
}




int main()
{
	//builder
	IGraphBuilder* m_pGraph;
	ICaptureGraphBuilder2* m_pBuild;
	IMediaControl * g_pMC = NULL;
	IMediaEventEx * g_pME = NULL;
	IVideoWindow  * g_pVW = NULL;
	AM_MEDIA_TYPE am_media_type;

	//src
	IBaseFilter* device = NULL;
	IPin *deviceout = NULL;

	//dst
	IBaseFilter* sample = NULL;
	IPin *samplein = NULL;
	ISampleGrabber* pGrabber = NULL;
	CallbackObject cb;

	//com init
	AM_MEDIA_TYPE mt;
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
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
    if (FAILED(hr)){printf("mediacontrol\n");return hr;}

    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &g_pVW);
    if (FAILED(hr)){printf("videowindow\n");return hr;}

    hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *) &g_pME);
    if (FAILED(hr)){printf("mediaevent\n");return hr;}




	//device filter
	hr = choose(&device, 1);
	if(hr < 0)goto fail;
	//printf("%llx\n",device);




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




	//connect, start
	hr = m_pGraph->AddFilter(device, L"Capture Filter");
	if(FAILED(hr)){printf("error %x@add device\n",hr);goto fail;}

	hr = m_pGraph->AddFilter(sample, L"Sample Grabber");
	if(FAILED(hr)){printf("error %x@add sample\n",hr);goto fail;}

	deviceout = GetOutPin(device, 0);
    samplein = GetInPin(sample, 0);
    hr = m_pGraph->Connect(deviceout, samplein);
	if(FAILED(hr)){printf("error %x@graph connect\n",hr);goto fail;}

	//hr = m_pBuild->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,device,NULL,NULL);
	//if(FAILED(hr)){printf("error %x@graph render\n",hr);goto fail;}

	hr = pGrabber->GetConnectedMediaType(&am_media_type);
	hr = g_pMC->Run();
	while(1)Sleep(1);




fail:
	//com uninit
	CoUninitialize();
end:
	return 0;
}
