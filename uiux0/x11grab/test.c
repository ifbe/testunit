#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
int main()
{
	Display* display = XOpenDisplay(NULL);
	printf("display=%p\n", display);
	if(0 == display)return -1;

	int screen = DefaultScreen(display);
	printf("screen=%x\n", screen);

	Window window = RootWindow(display, screen);
	printf("window=%x\n", window);

	XWindowAttributes attr;
	XGetWindowAttributes(display, window, &attr);

	int w = attr.width;
	int h = attr.height;
	printf("w=%d,h=%d\n", w, h);

	XImage* image = XGetImage(display, window, 0, 0, w, h, AllPlanes, ZPixmap);

	XCloseDisplay(display);
	return 0;
}
