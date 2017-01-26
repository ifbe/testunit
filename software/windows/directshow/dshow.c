#include <windows.h>
#include <dshow.h>
#pragma comment(lib, "ole32")
#pragma comment(lib, "strmiids")




int main()
{
	//enumerator
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;

	//builder
	IBaseFilter*           m_pSrc = NULL;
	IGraphBuilder*         m_pGraph;
	ICaptureGraphBuilder2* m_pBuild;
	HRESULT hr;

	//com init
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(!SUCCEEDED(hr))goto end;

	//Create the System Device Enumerator.
	hr = CoCreateInstance(
		CLSID_SystemDeviceEnum, NULL,  
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum)
	);
	if(!SUCCEEDED(hr))goto fail;

	//Create an enumerator for the category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
	if (hr == S_FALSE)printf("error@CreateClassEnumerator\n");

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;  
		} 

		VARIANT var;
		var.vt = VT_BSTR;

		// Get description or friendly name.
		hr = pPropBag->Read(L"Description", &var, 0);
		if (FAILED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &var, 0);
		}
		if (SUCCEEDED(hr))
		{
			printf("%S\n", var.bstrVal);
		}
/*
		hr = pPropBag->Read(L"DevicePath", &var, 0);
		if (SUCCEEDED(hr))
		{
			// The device path is not intended for display.
			printf("Device path: %S\n", var.bstrVal);
		}
*/
		pPropBag->Release();
		break;
	}

	//bind
	hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pSrc);
	if(!SUCCEEDED(hr))goto fail;

	//
	pMoniker->Release();
	pEnum->Release();
	pDevEnum->Release();

	//
	hr = CoCreateInstance(
		CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
		(void**)&m_pGraph
	);
	hr = CoCreateInstance(
		CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
		(void**)&m_pBuild
	);
	hr = m_pBuild->SetFiltergraph(m_pGraph);

	//add
	hr = m_pGraph->AddFilter(m_pSrc, L"Capture Filter");

	//
	hr = m_pBuild->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,m_pSrc,NULL,NULL);

fail:
	CoUninitialize();
end:
	return 0;
}
