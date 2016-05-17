#include <stdio.h>
#include<stdlib.h>
#include "SDL.h"
void userfunc();

SDL_Window* window;//窗口
SDL_Renderer* renderer;	//什么是render
							//surface是在内存里的？
SDL_Texture* texture;		//texture是在显存里的？
unsigned int* mypixel=NULL;
int xresolution;
int yresolution;


int main( int argc, char* args[] ) 
{
  //准备sdl
  SDL_Init(SDL_INIT_EVERYTHING);
  window=SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          0, 0,
                          SDL_WINDOW_FULLSCREEN_DESKTOP);
  renderer= SDL_CreateRenderer(window, -1, 0);
  texture= SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               400, 400);
  SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 0x44);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  
  
  //准备framebuffer
  mypixel=(unsigned int*)malloc(400*400*4);
  xresolution=400;
  yresolution=400;
  
  //进入用户程序
  userfunc();
  
  //等着被关
  SDL_Event event;
  while (1)
	{
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
			if (event.type == SDL_KEYDOWN) {
				break;
			}
        }
	}
  
  //释放framebuffer
  free(mypixel);
  
  
  //释放sdl
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window); 
  SDL_Quit(); 
  return 0;   
}








void point(int x,int y,int color)
{
	mypixel[y*xresolution+x]=color;
}
void draw(int x,int y,int color)
{
    point(x+(xresolution/2),(yresolution/2)-y,color);
}
void setscreen(int x,int y)
{
  //destory老的，create新的，清屏
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window); 
  window=SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          18,
                          x, y,
                          SDL_WINDOW_OPENGL);
  renderer= SDL_CreateRenderer(window, -1, 0);
  texture= SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               x, y);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  

  //free老的，malloc新的
  free(mypixel);
  mypixel=(unsigned int*)malloc(x*y*4);
  xresolution=x;
  yresolution=y;
}
void writescreen()
{
  //画texture？
  SDL_UpdateTexture(texture, NULL, mypixel, xresolution*4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
int waitevent()
{
    SDL_Event event;
	while (1)
	{
        //if (SDL_PollEvent(&event)) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                break;
            }
        }
	}
	return (int)(event.key.keysym.sym);
}