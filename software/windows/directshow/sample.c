#include<tchar.h>
#include<DShow.h>  
#include<Guiddef.h>  
#include<strmif.h>  
#include<windows.h>
#pragma comment(lib, "Dxguid.lib")  
#pragma comment(lib, "Strmiids.lib")  
#define CHECK_HR(s) if (FAILED(s)) {return 1;}  
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)
 



LRESULT CALLBACK WndMainProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    // Pass this message to the video window for notification of system changes
    //if (g_pVW)g_pVW->NotifyOwnerMessage((LONG_PTR) hwnd, message, wParam, lParam);

    return DefWindowProc (hwnd , message, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  
{
	HRESULT hr;
    IGraphBuilder*         m_pGraph=NULL;
    ICaptureGraphBuilder2* m_pBuild=NULL;

    ICreateDevEnum* pDevEnum=NULL;
    IEnumMoniker*   pClsEnum=NULL;
    IMoniker*       pMoniker=NULL;

    IBaseFilter*   m_pSrc=NULL;
    IVideoWindow*  m_pVidWin=NULL;
    IMediaControl* m_pMediaControl=NULL;




	//
    CoInitialize(NULL);

    hr = CoCreateInstance(
		CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
		(void**)&m_pGraph
	);  
    CHECK_HR(1);  
  
    hr = CoCreateInstance(
		CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
		(void**)&m_pBuild
	);  
    CHECK_HR(2);

    hr = m_pBuild->SetFiltergraph(m_pGraph);
    CHECK_HR(3);




    //创建设备枚举COM对象  
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC,IID_ICreateDevEnum,(void **)&pDevEnum);  
    CHECK_HR(5);  
  
    //创建视频采集设备枚举COM对象  
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pClsEnum,0);  
    CHECK_HR(6);  
  
    int i = 1;  
    while(i >= 0)  
    {  
        hr = pClsEnum->Next(1,&pMoniker,NULL);  
        i--;  
    }  
    CHECK_HR(7);  
  
    hr = pMoniker->BindToObject(0,0,IID_IBaseFilter,(void **)&m_pSrc);  
    CHECK_HR(8);  
  
    SAFE_RELEASE(pMoniker);  
    SAFE_RELEASE(pClsEnum);  
    SAFE_RELEASE(pDevEnum);  
  
    //将设备添加到管理器graph
    hr = m_pGraph->AddFilter(m_pSrc, L"Video Capture");  
    CHECK_HR(9);

    hr=m_pBuild->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,m_pSrc,NULL,NULL);  
    CHECK_HR(10);




	//
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof wc);
	wc.style=CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndMainProc;
    wc.hInstance     = 0;
    wc.lpszClassName = "test";
    wc.lpszMenuName  = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = NULL;
    RegisterClass(&wc);

	HWND win = CreateWindow("test", "test",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		640, 480,
		0, 0, 0, 0
	);
	ShowWindow(win,SW_SHOW);
	UpdateWindow(win);




	//
    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&m_pVidWin);
    CHECK_HR(4);

    m_pVidWin->put_Owner((OAHWND)win);
    m_pVidWin->SetWindowPosition(0, 0, 640, 480);
    m_pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	m_pVidWin->put_Visible(OATRUE);

    hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
    CHECK_HR(12);

    hr = m_pMediaControl->Run();

	//
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);  
        DispatchMessage(&msg);  
    }
	CoUninitialize();
}  
