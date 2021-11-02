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

int main()
{
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, 0, 0);

	return 0;
}
