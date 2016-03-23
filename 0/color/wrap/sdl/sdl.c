#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#undef main


unsigned int* mypixel;
int width=1024;
int height=512;

SDL_Window* window;//窗口
SDL_Renderer* renderer;	//什么是render
SDL_Texture* texture;		//texture是在显存里的？


__attribute__((constructor)) void initsdl()
{
  //准备sdl
  SDL_Init(SDL_INIT_EVERYTHING);
  window=SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          16,
                          width,height,
                          SDL_WINDOW_OPENGL);
  renderer= SDL_CreateRenderer(window, -1, 0);
  texture= SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               width,height);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
//printf("sdl inited\n");

  //准备framebuffer
  mypixel=(unsigned int*)malloc(width*height*4);
}
__attribute__((destructor)) void destorysdl()
{
  //释放sdl
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window); 
  SDL_Quit(); 
//printf("sdl destoryed\n");

  //释放framebuffer
  free(mypixel);
}




//上面几个试图初始化一些烦人的东西
//下面几个是提供使用的





void point(int x,int y,int color)
{
	mypixel[y*width+x]=color;
}
void draw(int x,int y,int color)
{
	point(x+(width/2),(height/2)-y-1,color);
}
void writescreen()
{
  //画texture？
  SDL_UpdateTexture(texture, NULL, mypixel, width*4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
int waitevent()
{
	SDL_Event event;
	while (1)
	{
		if (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return -1;
			}
			if (event.type == SDL_KEYDOWN)
			{
				int val=(int)(event.key.keysym.sym);
				//printf("%x\n",val);
				if(val == 0x40000057)val='+';
				if(val == 0x40000056)val='-';
				if(val == 0x40000055)val='*';
				if(val == 0x40000054)val='/';
				if(val == 0x40000058)val=0xd;
				return val;
			}
		}
	}
}