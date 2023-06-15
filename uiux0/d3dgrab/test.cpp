#include <windows.h>
BOOL ScreenShot(HWND hWnd, TCHAR* fileName) {
	HRESULT hr;
	IDirect3D9*   gpD3D=NULL;
	IDirect3DDevice9* gpd3dDevice=NULL;
	IDirect3DSurface9* gpSurface=NULL;
	D3DDISPLAYMODE ddm;
	D3DPRESENT_PARAMETERS d3dpp;

	if((gpD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL) {
		ErrorMessage("Unable to Create Direct3D ");
		return E_FAIL;
	}

	if(FAILED(gpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm))) {
		ErrorMessage("Unable to Get Adapter Display Mode");
		return E_FAIL;
	}

	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed=WINDOW_MODE;
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat=ddm.Format;
	d3dpp.BackBufferHeight=nDisplayHeight=gScreenRect.bottom =ddm.Height;
	d3dpp.BackBufferWidth=nDisplayWidth=gScreenRect.right =ddm.Width;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hWnd;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

	if(FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&d3dpp,&gpd3dDevice))) {
		ErrorMessage("Unable to Create Device");
		return E_FAIL;
	}

	if(FAILED(gpd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &gpSurface, NULL))) {
		ErrorMessage("Unable to Create Surface");
		return E_FAIL;
	}

	if (FAILED(hr = gpd3dDevice->GetFrontBufferData(0, gpSurface)))     {
		gpSurface->Release() ;
		return hr ;
	}

	hr = D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, gpSurface, NULL, NULL);
	gpSurface->Release() ;
	return hr ;
}
int main()
{
	ScreenShot(0, haha.jpg");
}