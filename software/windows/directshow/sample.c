#include "stdafx.h"  
#include <DShow.h>  
#include <Guiddef.h>  
#include <strmif.h>  
#pragma comment(lib, "Dxguid.lib")  
#pragma comment(lib, "Strmiids.lib")  
#define CHECK_HR(s) if (FAILED(s)) {return 1;}  
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)  
 



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  
{  
    PAINTSTRUCT ps;  
    HDC hdc;  
    TCHAR greeting[] = _T("Hello, World!");  
  
    switch (message)  
    {  
    case WM_PAINT:  
        hdc = BeginPaint(hWnd, &ps);  
        TextOut(hdc,5, 5,greeting, _tcslen(greeting));  
        EndPaint(hWnd, &ps);  
        break;  
    case WM_DESTROY:  
        PostQuitMessage(0);  
        break;  
    default:  
        return DefWindowProc(hWnd, message, wParam, lParam);  
        break;  
    }  
  
    return 0;  
}  
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  
{  
    WNDCLASSEX wcex;  
  
    wcex.cbSize = sizeof(WNDCLASSEX);  
    wcex.style          = CS_HREDRAW | CS_VREDRAW;  
    wcex.lpfnWndProc    = WndProc;  
    wcex.cbClsExtra     = 0;  
    wcex.cbWndExtra     = 0;  
    wcex.hInstance      = hInstance;  
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));  
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);  
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);  
    wcex.lpszMenuName   = NULL;  
    wcex.lpszClassName  = _T("aaaaa");  
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));  
  
    if (!RegisterClassEx(&wcex))  
    {  
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"),_T("Win32 Guided Tour"), NULL);  
  
        return 1;  
    }  
  
    IGraphBuilder         *m_pGraph;  
    ICaptureGraphBuilder2 *m_pBuild;  
    IBaseFilter           *m_pSrc;  
    IMediaControl         *m_pMediaControl;  
    IVideoWindow          *m_pVidWin;  
  
    CoInitialize(NULL);  
    int nRes = 0;  
    HRESULT hr =CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph);  
    CHECK_HR(1);  
  
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_pBuild);  
    CHECK_HR(2);  
  
    hr = m_pBuild->SetFiltergraph(m_pGraph);  
    CHECK_HR(3);  
  
    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&m_pVidWin);  
    CHECK_HR(4);  
  
    ICreateDevEnum *pDevEnum=NULL;  
    IEnumMoniker *pClsEnum=NULL;  
    IMoniker *pMoniker = NULL;  
    //创建设备枚举COM对象  
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC,IID_ICreateDevEnum,(void **)&pDevEnum);  
    CHECK_HR(5);  
  
    //创建视频采集设备枚举COM对象  
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pClsEnum,0);  
    CHECK_HR(6);  
  
    int i = 0;  
    while(i <= 0)  
    {  
        hr = pClsEnum->Next(1,&pMoniker,NULL);  
        ++i;  
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
  
    HWND m_hWnd = ::CreateWindowA("STATIC","ds_video_preview",WS_POPUP,100,100,500,500,NULL,NULL,NULL,NULL);  
    ShowWindow(m_hWnd,  nCmdShow);  
    UpdateWindow(m_hWnd);  
  
  
    if (m_hWnd == NULL)  
    {  
        nRes = 11;  
    }  
  
    m_pVidWin->put_Owner((OAHWND)m_hWnd);  
    m_pVidWin->SetWindowPosition(100, 100, 500, 500);  
    m_pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);  
  
    hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);  
    CHECK_HR(12);  
  
    hr = m_pMediaControl->Run();  
      
  
    MSG msg;  
    while (GetMessage(&msg, NULL, 0, 0))  
    {  
        TranslateMessage(&msg);  
        DispatchMessage(&msg);  
    }  
    return (int) msg.wParam;  
  
}  
