#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl31.h>
#include <assert.h>
#include <fcntl.h>
#include <gbm.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
static char COMPUTE_SHADER_SRC[] = 
"#version 310 es\n"
"layout(local_size_x=1,local_size_y=1,local_size_z=1) in;\n"
"layout(std430, binding = 0) readonly buffer Input0 {\n"
"    float data[];\n"
"} input0;\n"
"layout(std430, binding = 1) writeonly buffer Output {\n"
"    float data[];\n"
"} output0;\n"
"void main(){\n"
"    uint idx = gl_GlobalInvocationID.x;\n"
"    uint idy = gl_GlobalInvocationID.y;\n"
"    uint id = idx+idx+idx+idx+idy;\n"
"    output0.data[id] = input0.data[id] + 100.0;\n"
"}\n";


GLuint shader_makeobj(char* p)
{
/* setup a compute shader */
	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	assert(glGetError() == GL_NO_ERROR);
 
	const char *shader_source = p;
	glShaderSource(compute_shader, 1, &shader_source, NULL);
	assert(glGetError() == GL_NO_ERROR);
 
	glCompileShader(compute_shader);
	assert(glGetError() == GL_NO_ERROR);

	char message[256];
	glGetShaderInfoLog(compute_shader, 256, 0, message);	
	printf("compile:{%s\n}\n", message);

	return compute_shader;
}
void shader_rmobj(GLuint s)
{
	glDeleteShader(s);
}


GLuint shader_makeexe(GLuint compute_shader)
{
	GLuint shader_program = glCreateProgram();
 
	glAttachShader(shader_program, compute_shader);
	assert(glGetError() == GL_NO_ERROR);
 
	glLinkProgram(shader_program);
	assert(glGetError() == GL_NO_ERROR);
 
	char message[256];
	glGetProgramInfoLog(shader_program, 256, 0, message);	
	printf("link:{%s\n}\n", message);

	return shader_program;
}
void shader_rmexe(GLuint s)
{
	glDeleteProgram(s);
}


GLuint setupSSBufferObject(GLuint index, float* pIn, GLuint count)
{
	GLuint ssbo;
	glGenBuffers(1, &ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(float), pIn, GL_STATIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);

	return ssbo;
}


void test()
{
	#define arraysize 256
	float arraydata[arraysize];
	int x,y;
	for(x=0;x<arraysize;x++)arraydata[x] = (float)x;
	for(y=0;y<4;y++){
		for(x=0;x<3;x++){
			printf("%f, ", arraydata[y*4+x]);
		}
		printf("%f\n", arraydata[y*4+x]);
	}
	GLuint input0SSbo = setupSSBufferObject(0, arraydata, arraysize);
	GLuint outputSSbo = setupSSBufferObject(1, NULL, arraysize);
/*	int x,y;
	float data[16][16];
	for(y=0;y<16;y++){
		for(x=0;x<16;x++)data[y][x] = (float)(y*16+x);
	}

	GLuint t0 = texture_make(0, GL_READ_ONLY);
	GLuint t1 = texture_make(0, GL_WRITE_ONLY);

	glBindTexture(GL_TEXTURE_2D, t0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, 16,16, GL_RED, GL_FLOAT, data);
*/
	GLuint shader_obj = shader_makeobj(COMPUTE_SHADER_SRC);
	assert(shader_obj != 0);

	GLuint shader_program = shader_makeexe(shader_obj);
	assert(shader_program != 0);

	glUseProgram (shader_program);
	assert(glGetError () == GL_NO_ERROR);
 
	glDispatchCompute(4, 4, 1);
	assert(glGetError() == GL_NO_ERROR);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	printf("Compute shader dispatched and finished successfully\n");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputSSbo);
	float* pOut = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, arraysize * sizeof(float), GL_MAP_READ_BIT);
	for(y=0;y<4;y++){
		for(x=0;x<3;x++){
			printf("%f, ", pOut[y*4+x]);
		}
		printf("%f\n", pOut[y*4+x]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
/*
	glBindTexture(GL_TEXTURE_2D, t1);
	glGetTextureImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
	for(y=0;y<16;y++){
		printf("%d: ", y);
		for(x=0;x<16;x++)printf("%f ", data[y][x]);
		printf("\n");
	}
*/
	shader_rmexe(shader_program);
	shader_rmobj(shader_program);
/*
	glDeleteTextures(1, &t1);
	glDeleteTextures(1, &t0);
*/
}


int32_t
main (int32_t argc, char* argv[])
{
/* 
	int32_t fd = open ("/dev/dri/renderD128", O_RDWR);
	assert (fd > 0);
 
	struct gbm_device *gbm = gbm_create_device (fd);
	assert (gbm != NULL);
 
	EGLDisplay egl_dpy = eglGetPlatformDisplay (EGL_PLATFORM_GBM_MESA, gbm, NULL);
	assert (egl_dpy != NULL);
*/
	EGLDisplay* egl_dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	int err = eglGetError();
	if(EGL_NO_DISPLAY == egl_dpy || EGL_SUCCESS != err){
		return -1;
	}
	printf("display=%p\n", egl_dpy);

	int major,minor;
	int res = eglInitialize (egl_dpy, &major, &minor);
	assert (res);

	printf("EGL_VENDOR=%s\n",eglQueryString(egl_dpy, EGL_VENDOR));
	printf("EGL_VERSION=%s\n",eglQueryString(egl_dpy, EGL_VERSION));
	printf("EGL_CLIENT_APIS=%s\n",eglQueryString(egl_dpy, EGL_CLIENT_APIS));
	printf("EGL_EXTENSIONS=%s\n",eglQueryString(egl_dpy, EGL_EXTENSIONS));
	printf("....\n");
 
	const char *egl_extension_st = eglQueryString (egl_dpy, EGL_EXTENSIONS);
	assert (strstr (egl_extension_st, "EGL_KHR_create_context") != NULL);
	assert (strstr (egl_extension_st, "EGL_KHR_surfaceless_context") != NULL);
 
	static const EGLint config_attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
		EGL_NONE
	};
	EGLConfig cfg;
	EGLint count;
 
	res = eglChooseConfig (egl_dpy, config_attribs, &cfg, 1, &count);
	assert (res);
 
	res = eglBindAPI (EGL_OPENGL_ES_API);
	assert (res);
 
	static const EGLint attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	EGLContext core_ctx = eglCreateContext(egl_dpy, cfg, EGL_NO_CONTEXT, attribs);
	assert (core_ctx != EGL_NO_CONTEXT);
 
	res = eglMakeCurrent (egl_dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, core_ctx);
	assert (res);
 
	printf("GL_VENDOR=%s\n",glGetString(GL_VENDOR));
	printf("GL_VERSION=%s\n",glGetString(GL_VERSION));
	printf("GL_RENDERER=%s\n",glGetString(GL_RENDERER));
	printf("GL_SHADING_LANGUAGE_VERSION=%s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("GL_EXTENSIONS=%s\n",glGetString(GL_EXTENSIONS));
	printf("....\n");


	test();

	/* free stuff */
	eglDestroyContext (egl_dpy, core_ctx);
	eglTerminate (egl_dpy);
/*
	gbm_device_destroy (gbm);
	close (fd);
*/
	return 0;
}
