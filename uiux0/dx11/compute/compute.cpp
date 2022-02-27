#include <string>
#include <Windows.h>
#include <D3D11.h>
#include <d3dcompiler.h>
using namespace std;

//wnd thing
HINSTANCE	g_hInstance(NULL);
//d3d thing
ID3D11Device*           g_dx11device(NULL);
ID3D11DeviceContext*    g_dx11context(NULL);
//ctx
ID3D11ComputeShader* g_cs;
ID3D11UnorderedAccessView* g_uav;
ID3D11Buffer* g_d3d11buffer;

//my own
char cshader[] =
"struct BufferType{"
"	int val;"
"};"
//"StructuredBuffer<BufferType> inbuf : register(t0);"
"RWStructuredBuffer<BufferType> outbuf : register(u0);"
"[numthreads(4,4,1)]"
"void main(uint3 id:sv_dispatchthreadid){"
"	outbuf[id.y*16 + id.x].val += 800;"
"}";



void printadapter(IDXGIAdapter* adapter)
{
	DXGI_ADAPTER_DESC desc;
	adapter->GetDesc(&desc);

	char str[128];
	WideCharToMultiByte(CP_ACP, 0, desc.Description, -1, str, 128, NULL, FALSE);
	printf("Description=%s\n", str);

	printf("VendorId=%x,DeviceId=%x\n", desc.VendorId, desc.DeviceId);
	printf("SubSysId=%x,Revision=%x\n", desc.SubSysId, desc.Revision);
	printf("DedicatedVideoMemory=%p,DedicatedSystemMemory=%p,SharedSystemMemory=%p\n", desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory);
}
void enumadapter()
{
	IDXGIFactory* factory = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	UINT j;
	IDXGIAdapter* adapter;
	for(j=0;j<100;j++){
		if(DXGI_ERROR_NOT_FOUND == factory->EnumAdapters(j, &adapter))break;
		printf("enumadapter:%d{\n", j);
		printadapter(adapter);
		printf("}\n");
	}
}



void printd3d11device(ID3D11Device* d3d11device)
{
	IDXGIDevice* dxgidevice = NULL;
	d3d11device->QueryInterface(&dxgidevice);

	IDXGIAdapter* adapter = NULL;
	dxgidevice->GetAdapter( &adapter );

	printf("chosen adapter{\n");
	printadapter(adapter);
	printf("}\n");
}
void printresult(ID3D11Buffer* outbuf)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = 4*16*16;

	ID3D11Buffer* dbgbuf;
	HRESULT hr = g_dx11device->CreateBuffer(&desc, 0, &dbgbuf);
	if(FAILED(hr)){
		printf("CreateBuffer: dbgbuf fail\n");
		return;
	}

	g_dx11context->CopyResource(dbgbuf, outbuf);

	D3D11_MAPPED_SUBRESOURCE mapres;
	g_dx11context->Map(dbgbuf, 0, D3D11_MAP_READ, 0, &mapres);

	int* ptr = (int*)mapres.pData;
	int x,y;
	for(y=0;y<16;y++){
		for(x=0;x<15;x++)printf("%03d ",ptr[y*16+x]);
		printf("%03d\n",ptr[y*16+16-1]);
	}
}




void FreeD3D11()
{
	g_dx11context->Release();
	g_dx11device->Release();
}
BOOL InitD3D11()
{
	enumadapter();

	//create device and context
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

	printd3d11device(g_dx11device);

	return TRUE;
}



void Freemyctx()
{
}
int Initmyctx()
{
	ID3DBlob* csBlob = NULL;
	ID3DBlob* csError= NULL;
	HRESULT hr = D3DCompile(
		cshader, sizeof(cshader), "cs",
		0, 0,	//define, include
		"main", "cs_5_0",	//entry, target
		0, 0,	//flag1, flag2
		&csBlob, &csError
	);
	if(FAILED(hr)){
		MessageBox(NULL, (char*)csError->GetBufferPointer(), "D3DCompile(cshader)",MB_OK);
		return 0;
	}

	hr = g_dx11device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &g_cs );
	if(FAILED(hr)){
		csBlob->Release();
		return hr;
	}


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = 4*16*16;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = 4;

	int x,y;
	int initdata[16*16];
	for(y=0;y<16;y++){
		for(x=0;x<16;x++)initdata[y*16+x] = y*16+x;
	}
	for(y=0;y<16;y++){
		for(x=0;x<15;x++)printf("%03d ",initdata[y*16+x]);
		printf("%03d\n",initdata[y*16+16-1]);
	}

	D3D11_SUBRESOURCE_DATA subres;
	subres.pSysMem = initdata;
	hr = g_dx11device->CreateBuffer(&desc, &subres, &g_d3d11buffer);
	if(FAILED(hr)){
		printf("CreateBuffer: uavbuf fail\n");
		return 0;
	}
/*
	D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc;
	ZeroMemory(&viewdesc, sizeof(viewdesc));
	viewdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	viewdesc.BufferEx.FirstElement = 0;
	viewdesc.Format = DXGI_FORMAT_UNKNOWN;
	viewdesc.BufferEx.NumElements = 8*8;

	ID3D11ShaderResourceView* srv;
	g_dx11device->CreateShaderResourceView(d3d11buffer, &viewdesc, &srv);
*/
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavdesc;
	ZeroMemory(&uavdesc, sizeof(uavdesc));
	uavdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavdesc.Buffer.FirstElement = 0;
	uavdesc.Format = DXGI_FORMAT_UNKNOWN;
	uavdesc.Buffer.NumElements = 16*16;
	g_dx11device->CreateUnorderedAccessView(g_d3d11buffer, &uavdesc, &g_uav);

	return 1;
}




void compute()
{
	printf("--------compute setup------------\n");
	g_dx11context->CSSetShader(g_cs, 0, 0);
	//g_dx11context->CSSetConstantBuffers(0, 1, &csv);
	//g_dx11context->CSSetShaderResources(0, 1, &srv);
	g_dx11context->CSSetUnorderedAccessViews(0, 1, &g_uav, NULL);
	g_dx11context->Dispatch(3, 2, 1);
	printf("--------compute finish------------\n");

	Sleep(1);
	printresult(g_d3d11buffer);
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	g_hInstance = hInstance;

	if(!InitD3D11())return -1;
	if(!Initmyctx())return -1;

	compute();

	Freemyctx();
	FreeD3D11();
	return 0;
}
