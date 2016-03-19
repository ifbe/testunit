#include<stdio.h>
#include<stdlib.h>

#include<windows.h>
#include<Commctrl.h>
#include<Winuser.h>


int width=1024;
int height=512;
unsigned int* mypixel;

HWND window;
HDC realdc;
BITMAPINFO info;
MSG msg;









//inner
//outer



void point(int x,int y,int color)
{
	mypixel[y*width+x]=color;
}
void draw(int x,int y,int color)
{
	point(x+(width/2),(height/2)-y-1,color);
}
void whereisscreenbuf(unsigned long long* p)
{
	*p=(unsigned long long)mypixel;
}
void writescreen()
{
	//int result=SetDIBitsToDevice(realdc,
	SetDIBitsToDevice(realdc,
			0,0,		//目标位置x,y
			width,height,		//dib宽,高
			0,0,		//来源起始x,y
			0,height,			//起始扫描线,数组中扫描线数量,
			mypixel,		//rbg颜色数组
			&info,			//bitmapinfo
			DIB_RGB_COLORS);		//颜色格式
	//printf("result:%x\n",result);
}




// Step 3: the Window Procedure
int solved=1;
int my1;
int my2;
LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
		case WM_KEYDOWN:
		{
			switch(wparam)
			{
				case VK_UP:
				case VK_LEFT:
				case VK_RIGHT:
				case VK_DOWN:
				{
					solved=0;
					my1=1;
					my2=wparam;
				}
			}
			return 0;
		}
		case WM_CHAR:		//键盘点下
		{
			solved=0;
			my1=1;
			my2=wparam;
			return 0;
		}
		case WM_LBUTTONDOWN:		//鼠标左键点下
		{
			solved=0;
			my1=2;
			my2=lparam;
			return 0;
		}
		case WM_PAINT:		//显示
		{
			writescreen();
			return DefWindowProc(window, msg, wparam, lparam);
		}
		/*
		case WM_CLOSE:
		{
			printf("WM_CLOSE\n");
			return DefWindowProc(window, msg, wparam, lparam);
		}
		*/
		case WM_DESTROY:		//摧毁
		{
			//Shell_NotifyIcon(NIM_DELETE, &nid);
			//ReleaseDC(window,fakedc);  
			printf("WM_DESROY\n");
			PostQuitMessage(0);
			return 0;
		}
		/*
		case WM_QUIT:		//摧毁
		{
			printf("WM_QUIT\n");
			return DefWindowProc(window, msg, wparam, lparam);
		}
		*/
		default:
		{
			return DefWindowProc(window, msg, wparam, lparam);
		}
	}
}
int waitevent(unsigned int* first,unsigned int* second)
{
	//要是能收到消息就一直收+处理
	while(GetMessage(&msg,NULL,0,0))
	{
		//交给WindowProc，试着处理看看
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//WindowProc处理不了，交给用户来处理
		if(solved==0)
		{
			*first=my1;
			*second=my2;
			solved=1;
			return;
		}
	}

	//要是收不到消息就返回"关闭"
	*first=0;
}




//int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
int initwindow()
{
    //Step 1: Registering the Window Class+Creating the Window
    char *AppTitle="i am groot!";
    WNDCLASS wc;
    wc.style=CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc=WindowProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=0;				//hInst;
    wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)COLOR_WINDOWFRAME;
    wc.lpszMenuName=NULL;
    wc.lpszClassName=AppTitle;
    if (!RegisterClass(&wc)) return 0;

	window = CreateWindow(AppTitle,AppTitle,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,CW_USEDEFAULT,width+16,height+40,
				NULL,NULL,0,NULL);		//NULL,NULL,hInst,NULL);
    if (!window) return 0;

    ShowWindow(window,SW_SHOW);			//nCmdShow);
    UpdateWindow(window);
}
void initdib()
{
	//构造info
	info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth=width;
	info.bmiHeader.biHeight=-height;
	info.bmiHeader.biPlanes=1;
	info.bmiHeader.biBitCount=32;
	info.bmiHeader.biCompression=0;
	info.bmiHeader.biSizeImage=width*height*4;
	info.bmiHeader.biXPelsPerMeter=0;
	info.bmiHeader.biYPelsPerMeter=0;
	info.bmiHeader.biClrUsed=0;
	info.bmiHeader.biClrImportant=0;

	info.bmiColors[0].rgbBlue=255;
	info.bmiColors[0].rgbGreen=255;
	info.bmiColors[0].rgbRed=255;
	info.bmiColors[0].rgbReserved=255;
}
__attribute__((constructor)) void initsdl()
{
	//准备beforemain
	initwindow();

	//dib
	initdib();

	//拿dc
	realdc=GetDC(window);

	//准备rgb点阵
	mypixel=(unsigned int*)malloc(width*height*4);

	printf("initer called\n");
}
__attribute__((destructor)) void destorysdl()
{
	//释放点阵
	free(mypixel);

	//释放dc
	ReleaseDC(window,realdc);

	printf("destoryer called\n");
}