#include <stdio.h>
#include <windows.h>

#define TIMER_INTERVAL 1000
#define ID_STATIC_CONTROL 1001
#define ID_STATIC_CONTROL2 1002
static int count = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
        if (wParam == 1)
        {
            HWND hStaticControl = GetDlgItem(hWnd, ID_STATIC_CONTROL);
            HWND hStaticControl2 = GetDlgItem(hWnd, ID_STATIC_CONTROL2);
            char tmp[128];
            snprintf(tmp,128, "%d\nhaha\n", count);
            SetWindowTextA(hStaticControl, tmp);
            SetWindowTextA(hStaticControl2, tmp);
            count++;
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hWnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXA wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "StaticControlExample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassExA(&wcex)){
        MessageBoxA(NULL, "RegisterClassExA", "failed", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hWnd = CreateWindowExA(0, "StaticControlExample", "Static Control Example", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL);
    if (!hWnd){
        MessageBoxA(NULL, "CreateWindowExA rootwindow", "failed", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hStaticControl = CreateWindowExA(0, "STATIC", "initial context", WS_CHILD | WS_VISIBLE,
        0, 0, 640, 100,
        hWnd, (HMENU)ID_STATIC_CONTROL, hInstance, NULL);
    if (!hStaticControl){
        MessageBoxA(NULL, "CreateWindowExA staticview", "failed", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hStaticControl2 = CreateWindowExA(0, "STATIC", "initial context2", WS_CHILD | WS_VISIBLE,
        0, 200, 640, 100,
        hWnd, (HMENU)ID_STATIC_CONTROL2, hInstance, NULL);
    if (!hStaticControl){
        MessageBoxA(NULL, "CreateWindowExA staticview", "failed", MB_OK | MB_ICONERROR);
        return 0;
    }

    SetTimer(hWnd, 1, TIMER_INTERVAL, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}
