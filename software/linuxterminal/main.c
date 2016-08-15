#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
static char tab[0x100000];




void gotoxy(int x,int y)   //Fantasy  
{  
	printf("%c[%d;%df",0x1B,y,x);  
}  
int main (void)
{
	char ch;
	int x,y;
	int width,height;
	struct winsize w;

	ioctl(0, TIOCGWINSZ, &w);
	width=w.ws_row;
	height=w.ws_col;
	printf ("lines %d\n", w.ws_row);
	printf ("columns %d\n", w.ws_col);

	ch=0;
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

		sleep(1);
		ch = (ch+1)%0x50;
	}
}
