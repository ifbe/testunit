#include <Windows.h>
#include <string>
#include <D3D11.h>
#include <d3dcompiler.h>
using namespace std;

//wnd thing
HINSTANCE	g_hInstance(NULL);

//d3d thing
ID3D11Device*           g_dx11device(NULL);
ID3D11DeviceContext*    g_dx11context(NULL);

//my own
char cshader[] =
"}";




void compute()
{
}




void Freemyctx()
{
}
int Initmyctx()
{
	return 1;
}




void FreeD3D11()
{
	g_dx11context->Release();
	g_dx11device->Release();
}
BOOL InitD3D11()
{
	// a.创建设备和上下文
	D3D_FEATURE_LEVEL myFeatureLevel;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(
		NULL,				// 默认显示适配器
		D3D_DRIVER_TYPE_HARDWARE,
		0,				// 不使用软件设备
		createDeviceFlags,
		NULL, 0,			// 默认的特征等级数组
		D3D11_SDK_VERSION,
		&g_dx11device,
		&myFeatureLevel,
		&g_dx11context
	);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create d3d11 device failed!", "error",MB_OK);
		return FALSE;
	}

	return TRUE;
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	g_hInstance = hInstance;

	if(!InitD3D11())return -1;
	if(!Initmyctx())return -1;

	compute();

	Freemyctx()
	FreeD3D11();
	return 0;
}
