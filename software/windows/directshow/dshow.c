#include<dshow.h>
#include<windows.h>




ICaptureGraphBuilder2* pBuild;	//Capture Graph Builder
IGraphBuilder* pGraph;
IBaseFilter* pCap;		//capture filter
void main()
{
	HRESULT hr;

	hr = CoCreateInstance(
		CLSID_CaptureGraphBuilder2,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2,
		(void*)&pBuild
	);
	if(!SUCCEEDED(hr))return;

	hr = CoCreateInstance(
		CLSID_FilterGraph,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,
		(void*)&pGraph
	);
	if(!SUCCEEDED(hr))return;

	pBuild->SetFiltergraph(pGraph);
	pBuild->RenderStream(
		&PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Video,
		pCap, NULL, NULL
	);
}
