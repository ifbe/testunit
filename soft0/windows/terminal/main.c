#include <stdio.h>
#include <windows.h>
//
static HANDLE h;
//
static char tab[0x100000];




void gotoxy(int x,int y)
{
	COORD pos = {x,y};
	SetConsoleCursorPosition(h,pos);
}
int main (void)
{
	char ch;
	int x,y;
	int width,height;
	CONSOLE_SCREEN_BUFFER_INFO bInfo;

	h = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(h, &bInfo );
	width = bInfo.srWindow.Right - bInfo.srWindow.Left + 1;
	height = bInfo.srWindow.Bottom - bInfo.srWindow.Top + 1;

	ch = 0;
	while(1)
	{
		for(y=0;y<height;y++)
		{
			for(x=0;x<width;x++)
			{
				tab[y*width + x] = 0x30 + ch % 0x60;
			}
		}
		tab[height*width]=0;

		gotoxy(0,0);
		printf("%s",tab);
		fflush(stdout);

		Sleep(1);
		ch = (ch+1)%0x50;
	}
}
