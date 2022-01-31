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
	for(j=0;j<cnt;j++){
		eglGetConfigAttrib(display, cfg[j], EGL_RED_SIZE, &red);
		eglGetConfigAttrib(display, cfg[j], EGL_GREEN_SIZE, &green);
		eglGetConfigAttrib(display, cfg[j], EGL_BLUE_SIZE, &blue);
		eglGetConfigAttrib(display, cfg[j], EGL_ALPHA_SIZE, &alpha);

		eglGetConfigAttrib(display, cfg[j], EGL_DEPTH_SIZE, &depth);
		eglGetConfigAttrib(display, cfg[j], EGL_STENCIL_SIZE, &stencil);

		printf("%d:r%dg%db%da%d,d%ds%d\n", j,
			red,green,blue,alpha, depth,stencil);
	}

	eglTerminate(display);
	return 0;
}
