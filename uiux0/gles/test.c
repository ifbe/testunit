#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <errno.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
typedef unsigned char u8;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLContext context = EGL_NO_CONTEXT;
static EGLSurface surface = EGL_NO_SURFACE;

char* err2str(int err)
{
	switch(err){
	case EGL_SUCCESS:
		return "EGL_SUCCESS";
	case EGL_NOT_INITIALIZED:
		return "EGL_NOT_INITIALIZED";
	case EGL_BAD_ACCESS:
		return "EGL_BAD_ACCESS";
	case EGL_BAD_ALLOC:
		return "EGL_BAD_ALLOC";
	case EGL_BAD_ATTRIBUTE:
		return "EGL_BAD_ATTRIBUTE";
	case EGL_BAD_CONTEXT:
		return "EGL_BAD_CONTEXT";
	case EGL_BAD_CONFIG:
		return "EGL_BAD_CONFIG";
	case EGL_BAD_CURRENT_SURFACE:
		return "EGL_BAD_CURRENT_SURFACE";
	case EGL_BAD_DISPLAY:
		return "EGL_BAD_DISPLAY";
	case EGL_BAD_SURFACE:
		return "EGL_BAD_SURFACE";
	case EGL_BAD_MATCH:
		return "EGL_BAD_MATCH";
	case EGL_BAD_PARAMETER:
		return "EGL_BAD_PARAMETER";
	case EGL_BAD_NATIVE_PIXMAP:
		return "EGL_BAD_NATIVE_PIXMAP";
	case EGL_BAD_NATIVE_WINDOW:
		return "EGL_BAD_NATIVE_WINDOW";
	case EGL_CONTEXT_LOST:
		return "EGL_CONTEXT_LOST";
	}
	return "unknown";
}


char* table = ".'`^\",:;Il!i><~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
int ascii4y(int y)
{
	return table[((y&0xff)*0x44)>>8];
}
void rgbx_to_ascii(
	u8* srcbuf, int srclen, int srcw, int srch)
{
	int x,y,dx,dy;
	int avgy,avgu,avgv;
	int avgr,avgg,avgb;
	for(y=0;y<32;y++){
		for(x=0;x<64;x++){
			if(x*16+8 > srcw)continue;
			if(y*32+16 > srch)continue;
			avgr = 0;
			avgg = 0;
			avgb = 0;
			for(dy=0;dy<32;dy++){
				for(dx=0;dx<16;dx++){
					avgr += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+0];
					avgg += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+1];
					avgb += srcbuf[4*(srcw*(y*32+dy)+(x*16+dx))+2];
				}
			}
			avgr = avgr >> 7;
			avgg = avgg >> 7;
			avgb = avgb >> 7;
			avgy = ( 77*avgr +150*avgg + 29*avgb)>>8;
			printf("%c",ascii4y(avgy));
		}
		printf("\n");
	}
}


int main()
{
	printf("EGL_DEFAULT_DISPLAY=%llx\n",EGL_DEFAULT_DISPLAY);

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	int err = eglGetError();
	if(EGL_NO_DISPLAY == display || EGL_SUCCESS != err){
		printf("eglGetDisplay:err=%s\n", err2str(err));
		return -1;
	}
	printf("display=%p\n", display);

	int major,minor;
	int ret = eglInitialize(display, &major, &minor);
	err = eglGetError();
	if(EGL_SUCCESS != err){
		printf("eglInitialize:err=%s\n", err2str(err));
		return -1;
	}
	printf("eglInitialize: major=%d,minor=%d\n", major, minor);

	printf("EGL_VENDOR=%s\n",eglQueryString(display, EGL_VENDOR));
	printf("EGL_VERSION=%s\n",eglQueryString(display, EGL_VERSION));
	printf("EGL_EXTENSIONS=%s\n",eglQueryString(display, EGL_EXTENSIONS));

	int cnt;
	eglGetConfigs(display, 0, 0, &cnt);
	printf("cnt=%d\n", cnt);

	EGLConfig cfg[cnt];
	eglGetConfigs(display, cfg, cnt, &cnt);

	int j;
	int red,green,blue,alpha;
	int depth,stencil;
	int surfacetype;
	for(j=0;j<cnt;j++){
		eglGetConfigAttrib(display, cfg[j], EGL_RED_SIZE, &red);
		eglGetConfigAttrib(display, cfg[j], EGL_GREEN_SIZE, &green);
		eglGetConfigAttrib(display, cfg[j], EGL_BLUE_SIZE, &blue);
		eglGetConfigAttrib(display, cfg[j], EGL_ALPHA_SIZE, &alpha);

		eglGetConfigAttrib(display, cfg[j], EGL_DEPTH_SIZE, &depth);
		eglGetConfigAttrib(display, cfg[j], EGL_STENCIL_SIZE, &stencil);
		eglGetConfigAttrib(display, cfg[j], EGL_SURFACE_TYPE, &surfacetype);

		printf("%d: cfg=%x, r%dg%db%da%d, d%ds%d, %x\n", j, cfg[j],
			red,green,blue,alpha, depth,stencil, surfacetype);
	}


	EGLConfig config;
	EGLint numConfigs;
	const EGLint configAttribs[] = {
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			//EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_STENCIL_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_NONE
	};
	eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);
	printf("config=%x\n",config);

	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	printf("format=%x\n",format);


	EGLSurface surface = EGL_NO_SURFACE;
	if(0){
		surface = eglCreateWindowSurface(display, config, 0, NULL);
		if(EGL_NO_SURFACE == surface){
			printf("@eglCreateWindowSurface: err=%s\n", err2str(eglGetError()));
			return 0;
		}
	}
	else{
		EGLint pbufferAttribs[6];
		pbufferAttribs[0] = EGL_WIDTH;
		pbufferAttribs[1] = 1024;
		pbufferAttribs[2] = EGL_HEIGHT;
		pbufferAttribs[3] = 1024;
		pbufferAttribs[4] = EGL_NONE;
		surface = eglCreatePbufferSurface(display, config, pbufferAttribs);
		if(EGL_NO_SURFACE == surface){
			printf("@eglCreatePbufferSurface: err=%s\n", err2str(eglGetError()));
			return 0;
		}
	}
	printf("surface=%p\n",surface);


	EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if(EGL_NO_CONTEXT == context) {
		printf("@eglCreateContext: err=%s\n", err2str(eglGetError()));
		return 0;
	}
	printf("context=%p\n",context);


	ret = eglMakeCurrent(display, surface, surface, context);
       	if(ret == EGL_FALSE) {
		printf("@eglMakeCurrent: err=%s\n", err2str(ret));
		return 0;
	}


	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, 512, 512);
	glScissor(0, 0, 512, 512);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(256, 256, 512, 512);
	glScissor(256, 256, 512, 512);
	glClearColor(0.9, 0.9, 0.9, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	ret = eglSwapBuffers(display, surface);
       	if(ret != EGL_TRUE){
		printf("@eglSwapBuffers: err=%s\n", err2str(ret));
		//return 0;
	}


	//pbuffersurface
	int pixw = 1024;
	int pixh = 1024;
	GLubyte* data = (GLubyte*)malloc(pixw*pixh*4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(0,0,pixw,pixh,GL_RGBA,GL_UNSIGNED_BYTE,data);
	//printf("glReadPixels: err=%s\n", err2str(eglGetError()));

	rgbx_to_ascii(data, 0, 1024, 1024);

	free(data);


	if (context != EGL_NO_CONTEXT)eglDestroyContext(display, context);
	if (surface != EGL_NO_SURFACE)eglDestroySurface(display, surface);
	eglTerminate(display);
	return 0;
}
