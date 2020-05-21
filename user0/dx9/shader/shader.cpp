#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <wrl.h>
#include <vector>
 
using namespace Microsoft::WRL;
 
ComPtr<IDirect3D9> _d3d{ };
ComPtr<IDirect3DDevice9> _device{ };
ComPtr<IDirect3DVertexBuffer9> _vertexBuffer{ };
ComPtr<IDirect3DIndexBuffer9> _indexBuffer{ };
ComPtr<IDirect3DVertexDeclaration9> _vertexDecl{ };
ComPtr<IDirect3DVertexShader9> _vertexShader{ };
ComPtr<IDirect3DPixelShader9> _pixelShader{ };
ComPtr<ID3DXConstantTable> _vertexTable{ };
D3DXMATRIX _worldMatrix{ };
D3DXMATRIX _rotationMatrix{ };
 
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
    D3DXMatrixIdentity(&_worldMatrix);
    D3DXMatrixIdentity(&_rotationMatrix);
   
    D3DXMATRIX scaling{ };
    D3DXMatrixScaling(&scaling, 10, 10, 1);
 
    D3DXMatrixMultiply(&_worldMatrix, &scaling, &_rotationMatrix);
    HRESULT result = _vertexTable->SetMatrix(_device.Get(), "worldMatrix", &_worldMatrix);
    if (FAILED(result))
        return false;
 
    D3DXMATRIX view{ };
    D3DXVECTOR3 at = D3DXVECTOR3{ 0.0f, 0.0f, -20.0f };
    D3DXVECTOR3 to = D3DXVECTOR3{ 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3 up = D3DXVECTOR3{ 0.0f, 1.0f, 0.0f };
    D3DXMatrixLookAtLH(&view, &at, &to, &up);
    result = _vertexTable->SetMatrix(_device.Get(), "viewMatrix", &view);
    if (FAILED(result))
        return false;
 
    D3DXMATRIX projection{ };
    D3DXMatrixPerspectiveFovLH(&projection, D3DXToRadian(60.0f), 1024.0f / 768.0f, 0.0f, 100.0f);
    result = _vertexTable->SetMatrix(_device.Get(), "projectionMatrix", &projection);
    if (FAILED(result))
        return false;
 
    D3DXMatrixRotationY(&_rotationMatrix, D3DXToRadian(0.5f));
 
    return true;
}
 
IDirect3DVertexBuffer9* CreateVertexBuffer(const std::vector<VStruct>& vertices) {
    IDirect3DVertexBuffer9* buffer{ };
    HRESULT result{ };
   
    result = _device->CreateVertexBuffer(GetByteSize(vertices), 0, 0, D3DPOOL_DEFAULT, &buffer, nullptr);
    if (FAILED(result))
        return nullptr;
 
    void* data{ };
    result = buffer->Lock(0, GetByteSize(vertices), &data, 0);    
    if (FAILED(result))
        return nullptr;
 
    memcpy(data, vertices.data(), GetByteSize(vertices));
    buffer->Unlock();
 
    return buffer;
}
 
IDirect3DIndexBuffer9* CreateIndexBuffer(std::vector<unsigned int>& indices) {
    IDirect3DIndexBuffer9* buffer{ };
    HRESULT result{ };
   
    result = _device->CreateIndexBuffer(GetByteSize(indices), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &buffer, nullptr);
    if (FAILED(result))
        return nullptr;
 
    void* data{ };
    result = buffer->Lock(0, GetByteSize(indices), &data, 0);
    if (FAILED(result))
        return nullptr;
 
    memcpy(data, indices.data(), GetByteSize(indices));
    buffer->Unlock();
 
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
 
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
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
        VStruct{  1.0f,  1.0f, 1.0f, D3DXCOLOR{ 0.0f, 0.0f, 1.0f, 1.0f } },
        VStruct{  1.0f, -1.0f, 1.0f, D3DXCOLOR{ 1.0f, 1.0f, 0.0f, 1.0f } }
    };
 
    std::vector<unsigned int> indices{
        0, 1, 2,
        0, 2, 3
    };
 
    if (!(_vertexBuffer = CreateVertexBuffer(vertices)))
        return -1;
 
    if (!(_indexBuffer = CreateIndexBuffer(indices)))
        return -1;
 
    HRESULT result = _device->SetStreamSource(0, _vertexBuffer.Get(), 0, sizeof(VStruct));
    if (FAILED(result))
        return -1;
 
    result = _device->SetIndices(_indexBuffer.Get());
    if (FAILED(result))
        return -1;
 
    std::vector<D3DVERTEXELEMENT9> vertexDecl{
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };
 
    result = _device->CreateVertexDeclaration(vertexDecl.data(), &_vertexDecl);
    if (FAILED(result))
        return -1;
 
    _device->SetVertexDeclaration(_vertexDecl.Get());
 
    D3DCAPS9 deviceCaps{ };
    _device->GetDeviceCaps(&deviceCaps);
 
    if (deviceCaps.VertexShaderVersion < D3DVS_VERSION(3, 0))
        return -1;
 
    if (deviceCaps.PixelShaderVersion < D3DPS_VERSION(3, 0))
        return -1;
   
    ID3DXBuffer* vertexShaderBuffer{ };
    ID3DXBuffer* pixelShaderBuffer{ };
 
    result = D3DXCompileShaderFromFile("vertex.hlsl", nullptr, nullptr, "main", "vs_3_0", 0, &vertexShaderBuffer, nullptr, &_vertexTable);
    if (FAILED(result))
        return -1;
 
    result = D3DXCompileShaderFromFile("pixel.hlsl", nullptr, nullptr, "main", "ps_3_0", 0, &pixelShaderBuffer, nullptr, nullptr);
    if (FAILED(result))
        return -1;
 
    result = _device->CreateVertexShader((DWORD*)vertexShaderBuffer->GetBufferPointer(), &_vertexShader);
    if (FAILED(result))
        return -1;
   
    result = _device->CreatePixelShader((DWORD*)pixelShaderBuffer->GetBufferPointer(), &_pixelShader);
    if (FAILED(result))
        return -1;
 
    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
 
    _device->SetVertexShader(_vertexShader.Get());
    _device->SetPixelShader(_pixelShader.Get());
 
    if (!SetupTransform())
        return -1;
 
    _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
 
    MSG msg{ };
    while (_running) {
        while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        _device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DXCOLOR{ 0.0f, 0.0f, 0.0f, 1.0f }, 0.0f, 0);  
 
        _device->BeginScene();
        _device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, indices.size(), 0, 2);
        _device->EndScene();
 
        _device->Present(nullptr, nullptr, nullptr, nullptr);
 
        D3DXMatrixMultiply(&_worldMatrix, &_worldMatrix, &_rotationMatrix);
        _vertexTable->SetMatrix(_device.Get(), "worldMatrix", &_worldMatrix);
    }
 
    return 0;
}
