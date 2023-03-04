#include <stdio.h>
#include <windows.h>

struct priv{
	HWND hwnd;
	RECT rect;

	HDC hdc1;
	HBITMAP bitmap;
	BITMAPINFO info;

	HDC hdc2;
	unsigned char* buf;
};
int gdigrab_init(struct priv* my){
	my->hwnd = GetDesktopWindow();
	printf("hwnd=%x\n", my->hwnd);

	GetWindowRect(my->hwnd, &my->rect);
	printf("lt=(%d,%d),rb=(%d,%d)\n", my->rect.left, my->rect.top, my->rect.right, my->rect.bottom);
	int width = my->rect.right - my->rect.left;
	int height= my->rect.bottom - my->rect.top;

	my->hdc1 = GetWindowDC(my->hwnd);
	printf("hdc1=%p\n", my->hdc1);
	my->bitmap = CreateCompatibleBitmap(my->hdc1, width, height);
	printf("bitmap=%p\n", my->bitmap);
	my->info = {{sizeof(BITMAPINFOHEADER), width, height, 1, 24, 0 ,0, 0, 0, 0, 0}, {}};
	printf("info=%p\n", my->info);

	my->hdc2 = CreateCompatibleDC(my->hdc1);
	printf("hdc2=%p\n", my->hdc2);
	SelectObject(my->hdc2, my->bitmap);

	my->buf = (unsigned char*)malloc(width*height*4);
	printf("buf=%p\n", my->buf);

	while(1){
		BitBlt(my->hdc2, 0, 0, width, height, my->hdc1, 0, 0, SRCCOPY);
		GetDIBits(my->hdc2, my->bitmap, 0, height, my->buf, &my->info, DIB_RGB_COLORS);
		printf("%x,%x,%x,%x\n",my->buf[0],my->buf[1],my->buf[2],my->buf[3]);
	}
	return 0;
}
int gdigrab_exit(struct priv* my){
	free(my->buf);
	return 0;
}
int main(){
	struct priv my;
	printf("%x\n",sizeof(struct priv));
	gdigrab_init(&my);
	gdigrab_exit(&my);
	return 0;
}
