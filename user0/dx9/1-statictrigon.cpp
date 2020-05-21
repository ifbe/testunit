#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <wrl.h>
#include <vector>
 
using namespace Microsoft::WRL;
 
const unsigned long VertexStructFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
 
ComPtr<IDirect3D9> _d3d{ };
ComPtr<IDirect3DDevice9> _device{ };
ComPtr<IDirect3DVertexBuffer9> _vertexBuffer{ };
 
bool _running{ false };
 
template <typename T>
unsigned int GetByteSize(const std::vector<T>& vec) {
    return sizeof(vec[0]) * vec.size();
}
 
struct VStruct {
    float x, y, z;
    D3DCOLOR color;
};
 
 
bool SetupTransform() {
    D3DXMATRIX view{ };
    D3DXVECTOR3 at = D3DXVECTOR3{ 0.0f, 0.0f, -20.0f };
    D3DXVECTOR3 to = D3DXVECTOR3{ 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3 up = D3DXVECTOR3{ 0.0f, 1.0f, 0.0f };
    D3DXMatrixLookAtLH(&view, &at, &to, &up);
   
    HRESULT result{ };
    result = _device->SetTransform(D3DTS_VIEW, &view);
    if (FAILED(result))
        return false;
 
    D3DXMATRIX projection{ };
    D3DXMatrixPerspectiveFovLH(&projection, D3DXToRadian(60.0f), 1024.0f / 768.0f, 0.0f, 100.0f);  
 
    result = _device->SetTransform(D3DTS_PROJECTION, &projection);
    if (FAILED(result))
        return false;
 
    result = _device->SetRenderState(D3DRS_LIGHTING, false);
 
    return true;
}
 
IDirect3DVertexBuffer9* CreateBuffer(const std::vector<VStruct>& vertices) {
    IDirect3DVertexBuffer9* buffer{ };
    HRESULT result{ };
   
    result = _device->CreateVertexBuffer(GetByteSize(vertices), 0, VertexStructFVF, D3DPOOL_DEFAULT, &buffer, nullptr);
    if (FAILED(result))
        return nullptr;
 
    void* data{ };
    result = buffer->Lock(0, GetByteSize(vertices), &data, 0);    
    if (FAILED(result))
        return nullptr;
 
    memcpy(data, vertices.data(), GetByteSize(vertices));
    buffer->Unlock();
 
    _device->SetFVF(VertexStructFVF);
   
    return buffer;
}
 
bool InitD3D(HWND hWnd) {
    RECT clientRect{ };
    GetClientRect(hWnd, &clientRect);
 
    _d3d = Direct3DCreate9(D3D_SDK_VERSION);
 
    D3DDISPLAYMODE displayMode{ };
    _d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
 
    D3DPRESENT_PARAMETERS pp{ };
    pp.BackBufferWidth = clientRect.right;
    pp.BackBufferHeight = clientRect.bottom;
    pp.BackBufferFormat = displayMode.Format;
    pp.BackBufferCount = 1;                
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = hWnd;              
    pp.Windowed = true;                    
    pp.Flags = 0;                    
 
    HRESULT result{ };
    result = _d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &_device);
    if (FAILED(result))
        return false;
 
    D3DVIEWPORT9 viewport{ };
    viewport.X = 0;      
    viewport.Y = 0;  
    viewport.Width = clientRect.right;
    viewport.Height = clientRect.bottom;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 100.0f;
 
    result = _device->SetViewport(&viewport);
    if (FAILED(result))
        return false;
 
    return true;
}
 
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            _running = false;
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wndEx{ };
    wndEx.cbSize = sizeof(WNDCLASSEX);
    wndEx.style = CS_VREDRAW | CS_HREDRAW;
    wndEx.lpfnWndProc = WndProc;              
    wndEx.cbClsExtra = 0;                      
    wndEx.cbWndExtra = 0;              
    wndEx.hInstance = hInstance;          
    wndEx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndEx.hCursor = LoadCursor(nullptr, IDC_ARROW);  
    wndEx.hbrBackground = (HBRUSH)COLOR_WINDOW;    
    wndEx.lpszMenuName = nullptr;                
    wndEx.lpszClassName = "DirectXClass";        
    wndEx.hIconSm = nullptr;            
 
    RegisterClassEx(&wndEx);
 
    HWND hWnd{ nullptr };
    hWnd = CreateWindow("DirectXClass", "directx window", WS_OVERLAPPEDWINDOW, 50, 50, 1024, 768, nullptr, nullptr, hInstance, nullptr);  
 
    if (!hWnd)
        return -1;
 
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
 
    _running = true;
 
    if (!InitD3D(hWnd))
        return -1;
 
    std::vector<VStruct> vertices{
        VStruct{ -1.0f, -1.0f, 1.0f, D3DXCOLOR{ 1.0f, 0.0f, 0.0f, 1.0f } },
        VStruct{ -1.0f,  1.0f, 1.0f, D3DXCOLOR{ 0.0f, 1.0f, 0.0f, 1.0f } },
        VStruct{  1.0f,  1.0f, 1.0f, D3DXCOLOR{ 0.0f, 0.0f, 1.0f, 1.0f } }
    };
   
    if (!(_vertexBuffer = CreateBuffer(vertices)))
        return -1;
 
    if (!SetupTransform())
        return -1;
 
    HRESULT result = _device->SetStreamSource(0, _vertexBuffer.Get(), 0, sizeof(VStruct));
    if (FAILED(result))
        return -1;
 
    D3DXMATRIX world{ };
    D3DXMatrixScaling(&world, 10, 10, 1);
    _device->SetTransform(D3DTS_WORLD, &world);
 
    MSG msg{ };
    while (_running) {
        while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        _device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DXCOLOR{ 0.0f, 0.0f, 0.0f, 1.0f }, 0.0f, 0);  
 
        _device->BeginScene();
        _device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        _device->EndScene();
 
        _device->Present(nullptr, nullptr, nullptr, nullptr);
    }
 
    return 0;
}
