#include <Windows.h>
#include <string>
#include <D3D11.h>
#include <d3dcompiler.h>
using namespace std;

//wnd thing
HINSTANCE	g_hInstance(NULL);
HWND		g_hWnd(NULL);
UINT		g_winWidth(640);
UINT		g_winHeight(480);

//d3d thing
ID3D11Device*           g_device(NULL);
ID3D11DeviceContext*    g_deviceContext(NULL);
IDXGISwapChain*         g_swapChain(NULL);
ID3D11DepthStencilView* g_depthStencilView(NULL);
ID3D11RenderTargetView* g_renderTargetView(NULL);

//my thing
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;

//
char vshader[] =
"struct VSin{\n"
	"float4 where : POSITION;\n"
	"float4 color : COLOR;\n"
"};\n"
"struct VSout{\n"
	"float4 where : SV_POSITION;\n"
	"float4 color : COLOR;\n"
"};\n"
"VSout main(VSin input){\n"
	"VSout output;\n"
	"output.where = input.where;\n"
	"output.color = input.color;\n"
	"return output;\n"
"}\n";
char pshader[] =
"struct PSin{\n"
"	float4 where : SV_POSITION;\n"
"	float4 color : COLOR;\n"
"};\n"
"float4 main(PSin input) : SV_TARGET{\n"
"	return input.color;\n"
"}";




void Render()
{
	// 绘制青色背景
	float color[4] = {0.f, 1.f, 1.f, 1.0f};
	g_deviceContext->ClearRenderTargetView(g_renderTargetView,reinterpret_cast<float*>(&color));
	g_deviceContext->ClearDepthStencilView(g_depthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.f,0);

	// 正式的场景绘制工作
	g_deviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_deviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	g_deviceContext->Draw(3, 0);

	// 显示
	g_swapChain->Present(0,0);
}
void Freemyctx()
{
}
int Initmyctx()
{
	HRESULT hr;

	// Compile the vertex shader
    ID3DBlob* VSBlob = NULL;
	ID3DBlob* VSError= NULL;
	hr = D3DCompile(
		vshader, sizeof(vshader), "vs",
		0, 0,	//define, include
		"main", "vs_5_0",	//entry, target
		0, 0,	//flag1, flag2
		&VSBlob, &VSError
	);
	if(FAILED(hr)){
		MessageBox(NULL, (char*)VSError->GetBufferPointer(), "D3DCompile(vshader)",MB_OK);
		return 0;
	}

	// Create the vertex shader
	hr = g_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if(FAILED(hr)){	
		VSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* PSBlob = NULL;
	ID3DBlob* PSError= NULL;
	hr = D3DCompile(
		pshader, sizeof(pshader), "ps",
		0, 0,	//define, include
		"main", "ps_5_0",	//entry, target
		0, 0,	//flag1, flag2
		&PSBlob, &PSError
	);
	if(FAILED(hr)){
		MessageBox(NULL, (char*)PSError->GetBufferPointer(), "D3DCompile(pshader)",MB_OK);
		return 0;
	}

	// Create the pixel shader
	hr = g_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &g_pPixelShader );
	if(FAILED(hr)){
		PSBlob->Release();
		return hr;
	}


	// 设置三角形顶点
	float vertices[][8] = {
		{  0.0f, 50.0f, 50.0f, 1.0, 0.0f, 1.0f, 0.0f, 1.0f},
		{ 50.0f,-50.0f, 50.0f, 1.0, 0.0f, 0.0f, 1.0f, 1.0f},
		{-50.0f,-50.0f, 50.0f, 1.0, 1.0f, 0.0f, 0.0f, 1.0f}
	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(Data));
	Data.pSysMem = vertices;
	hr = g_device->CreateBuffer(&vbd, &Data, &g_pVertexBuffer);
	if(FAILED(hr)){
		MessageBox(NULL, "CreateBuffer", "Error", MB_OK);
		return hr;
	}

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = 32;
	UINT offset = 0;
	g_deviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_deviceContext->IASetInputLayout(g_pVertexLayout);

	return 1;
}




void FreeD3D11()
{
	g_depthStencilView->Release();
	g_renderTargetView->Release();

	g_swapChain->Release();
	g_deviceContext->Release();
	g_device->Release();
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
		NULL,						// 默认显示适配器
		D3D_DRIVER_TYPE_HARDWARE,
		0,							// 不使用软件设备
		createDeviceFlags,
		NULL, 0,					// 默认的特征等级数组
		D3D11_SDK_VERSION,
		&g_device,
		&myFeatureLevel,
		&g_deviceContext);

	if(FAILED(hr))
	{
		MessageBox(NULL, "Create d3d11 device failed!", "error",MB_OK);
		return FALSE;
	}

	// b.4X多重采样质量等级
	UINT m4xMsaaQuality(0);
	g_device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&m4xMsaaQuality);

	// c.准备交换链属性
	DXGI_SWAP_CHAIN_DESC sd = {0};
	sd.BufferDesc.Width = g_winWidth;
	sd.BufferDesc.Height = g_winHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = m4xMsaaQuality-1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = g_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// d.创建交换链
	IDXGIDevice *dxgiDevice(NULL);
	g_device->QueryInterface(__uuidof(IDXGIDevice),(void**)(&dxgiDevice));
	IDXGIAdapter *dxgiAdapter(NULL);
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)(&dxgiAdapter));
	IDXGIFactory *dxgiFactory(NULL);
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory),(void**)(&dxgiFactory));
	hr = dxgiFactory->CreateSwapChain(g_device,&sd,&g_swapChain);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create swap chain failed!", "error",MB_OK);
		return FALSE;
	}
	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	// e.创建渲染目标视图
	ID3D11Texture2D *backBuffer(NULL);
	g_swapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(&backBuffer));
	hr = g_device->CreateRenderTargetView(backBuffer,NULL,&g_renderTargetView);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create render target view failed!", "error",MB_OK);
		return FALSE;
	}
	backBuffer->Release();

	// f.创建深度缓冲区和其视图
	D3D11_TEXTURE2D_DESC depthStencilDesc = {0};
	depthStencilDesc.Width				= g_winWidth;
	depthStencilDesc.Height				= g_winHeight;
	depthStencilDesc.MipLevels			= 1;
	depthStencilDesc.ArraySize			= 1;
	depthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count	= 4;
	depthStencilDesc.SampleDesc.Quality	= m4xMsaaQuality-1;
	depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags		= 0;
	depthStencilDesc.MiscFlags			= 0;

	ID3D11Texture2D *depthStencilBuffer(NULL);
	hr = g_device->CreateTexture2D(&depthStencilDesc,NULL,&depthStencilBuffer);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create depth stencil buffer failed!", "error",MB_OK);
		return FALSE;
	}
	hr = g_device->CreateDepthStencilView(depthStencilBuffer,NULL,&g_depthStencilView);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create depth stencil view failed!", "error",MB_OK);
		return FALSE;
	}

	// g.将视图绑定到输出合并器阶段
	g_deviceContext->OMSetRenderTargets(1,&g_renderTargetView,g_depthStencilView);
	depthStencilBuffer->Release();

	// h.设置视口
	D3D11_VIEWPORT vp = {0};
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width	= static_cast<float>(g_winWidth);
	vp.Height	= static_cast<float>(g_winHeight);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	g_deviceContext->RSSetViewports(1,&vp);

	return TRUE;
}




LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}
void FreeWin32()
{
}
BOOL InitWin32()
{
	WNDCLASS wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndcls.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndcls.hInstance = g_hInstance;
	wndcls.lpfnWndProc = WinProc;
	wndcls.lpszClassName = "d3d11";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	if(!RegisterClass(&wndcls)){
		MessageBox(NULL, "Register window failed!", "error",MB_OK);
		return FALSE;
	}

	g_hWnd = CreateWindow(
		"d3d11",
		"d3d11",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,
		g_winWidth,g_winHeight,
		NULL,
		NULL,
		g_hInstance,
		NULL
	);
	if(!g_hWnd){
		MessageBox(NULL, "Create window failed!", "error",MB_OK);
		return FALSE;
	}

	ShowWindow(g_hWnd,SW_SHOW);
	UpdateWindow(g_hWnd);

	return TRUE;
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	g_hInstance = hInstance;

	if(!InitWin32())return -1;
	if(!InitD3D11())return -1;
	if(!Initmyctx())return -1;

	MSG msg = {0};
	//主消息循环，也是游戏当中的主循环
	while(msg.message != WM_QUIT)
	{
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Render();
	}

	Freemyctx();
	FreeD3D11();
	FreeWin32();
	return 0;
}