#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define PI 3.1415926535897932384626433832795028841971693993151
//
int width;
int height;
int last_x=0;
int last_y=0;
//
GLuint vShader;
GLuint fShader;
GLuint programHandle;
//
GLuint axisvao;
GLuint axispositionhandle;
GLuint axisnormalhandle;
GLuint axiscolorhandle;
//
GLuint samplevao;
GLuint sampleindexhandle;
GLuint samplepositionhandle;
GLuint samplenormalhandle;
GLuint samplecolorhandle;
//
static float camera[4] = {2.0f, -2.0f, 2.0f};
static float center[4] = {0.0f, 0.0f, 0.0f};
static float above[4] = {0.0f, 0.0f, 1.0f};
//
GLfloat modelmatrix[4*4] = {  
	1.0f, 0.0f, 0.0f, 0.0f,  
	0.0f, 1.0f, 0.0f, 0.0f,  
	0.0f, 0.0f, 1.0f, 0.0f,  
	0.0f, 0.0f, 0.0f, 1.0f,  
};
GLfloat viewmatrix[4*4] = {  
	1.0f, 0.0f, 0.0f, 0.0f,  
	0.0f, 1.0f, 0.0f, 0.0f,  
	0.0f, 0.0f, 1.0f, 0.0f,  
	0.0f, 0.0f, 0.0f, 1.0f,  
};
GLfloat projmatrix[4*4] = {  
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f, -1.0f,
	0.0f, 0.0f, -0.2f, 0.0f
};
//
float axispositiondata[] = {
	-10.0, 0.0, 0.0,
	10.0, 0.0, 0.0,
	0.0, -10.0, 0.0,
	0.0, 10.0, 0.0,
	0.0, 0.0, -10.0,
	0.0, 0.0, 10.0
};
float axisnormaldata[] = {
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0
};
float axiscolordata[] = {
	1.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 0.0, 1.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 1.0,
	1.0, 0.0, 0.0
};
//
float samplepositiondata[] = {
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, 1.0, -1.0,
	-1.0, 1.0, -1.0,
	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,
	-1.0, 1.0, 1.0,
};
float samplenormaldata[] = {
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, 1.0, -1.0,
	-1.0, 1.0, -1.0,
	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,
	-1.0, 1.0, 1.0,
};
float samplecolordata[] = {
	0.0, 0.0, 0.0,
	0.0, 0.0, 1.0f,
	0.0, 1.0f, 0.0,
	0.0, 1.0f, 1.0f,
	1.0f, 0.0, 0.0,
	1.0f, 0.0, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f
};
unsigned short sampleindexdata[] = {
	0, 1, 2, 3,
	0, 1, 5, 4,
	5, 6, 2, 1,
	5, 6, 7, 4,
	3, 7, 4, 0,
	3, 7, 6, 2
};




char vCode[] = {
	"#version 400\n"
	"layout(location = 0)in vec3 position;\n"
	"layout(location = 1)in vec3 normal;\n"
	"layout(location = 2)in vec3 color;\n"
	"uniform vec3 ambientcolor;\n"
	"uniform vec3 lightcolor;\n"
	"uniform vec3 lightposition;\n"
	"uniform vec3 eyeposition;\n"
	"uniform mat4 modelviewproj;\n"
	"uniform mat4 normalmatrix;\n"
	"out vec3 vertexcolor;\n"
	"void main()\n"
	"{\n"
		"vec3 S = normalize(vec3(lightposition - position));\n"
		"vec3 N = normalize(normal);\n"
		"vec3 V = normalize(-position);\n"
		"vec3 R = reflect(-S, N);\n"
		"float SN = max(dot(S, N), 0.0);\n"
		"float RV = max(dot(R, V), 0.0);\n"
		"vec3 ambient = color * ambientcolor;\n"
		"vec3 diffuse = color * lightcolor * SN;\n"
		"vec3 specular = vec3(0.0, 0.0, 0.0);\n"
		"if(SN>0.0)specular = color * lightcolor * pow(RV, 8);\n"
		"vertexcolor = ambient + diffuse + specular;\n"
		"gl_Position = modelviewproj * vec4(position,1.0);\n"
	"}\n"
};
char fCode[] = {
	"#version 400\n"
	"in vec3 vertexcolor;\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
		"FragColor = vec4(vertexcolor,1.0);\n"
	"}\n"
};
void initShader()  
{  
    //1. 查看GLSL和OpenGL的版本  
    const GLubyte *renderer = glGetString( GL_RENDERER );  
    const GLubyte *vendor = glGetString( GL_VENDOR );  
    const GLubyte *version = glGetString( GL_VERSION );  
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );  
    GLint major, minor;  
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("GL Vendor: %s\n", vendor);
    printf("GL Renderer: %s\n", renderer);
    printf("GL Version (string): %s\n", version);
    printf("GLSL Version: %s\n", glslVersion);
    printf("GL Version (integer): %x.%x\n", major, minor);

    //2. 顶点着色器  
    vShader = glCreateShader(GL_VERTEX_SHADER);
    if (0 == vShader)  
    {  
        printf("ERROR : Create vertex shader failed\n");
        exit(1);  
    }  

    //把着色器源代码和着色器对象相关联
	const GLchar* vCodeArray[1] = {vCode};
    glShaderSource(vShader, 1, vCodeArray, NULL);
    glCompileShader(vShader);  

    //检查编译是否成功  
    GLint compileResult;  
    glGetShaderiv(vShader,GL_COMPILE_STATUS,&compileResult);  
    if (GL_FALSE == compileResult)  
    {  
        GLint logLen;  
        //得到编译日志长度  
        glGetShaderiv(vShader,GL_INFO_LOG_LENGTH,&logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            //得到日志信息并输出  
            glGetShaderInfoLog(vShader,logLen,&written,log);
            printf("vertex shader compile log: %s\n",log);
            free(log);//释放空间
        }
    }

    //3. 片断着色器  
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (0 == fShader)  
    {  
        printf("ERROR : Create fragment shader failed");  
        exit(1);  
    }  

    //把着色器源代码和着色器对象相关联
	const GLchar* fCodeArray[1] = {fCode};
    glShaderSource(fShader, 1, fCodeArray, NULL);
    glCompileShader(fShader);  

    //检查编译是否成功  
    glGetShaderiv(fShader,GL_COMPILE_STATUS,&compileResult);  
    if (GL_FALSE == compileResult)  
    {  
        GLint logLen;  
        //得到编译日志长度  
        glGetShaderiv(fShader,GL_INFO_LOG_LENGTH,&logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            //得到日志信息并输出  
            glGetShaderInfoLog(fShader,logLen,&written,log);
            printf("fragment shader compile log: %s\n",log);
            free(log);//释放空间  
        }  
    }  
  
    //4. 着色器程序  
    programHandle = glCreateProgram();  
    if (!programHandle)  
    {  
        printf("ERROR : create program failed");
        exit(1);  
    }

    //将着色器程序链接到所创建的程序中  
    glAttachShader(programHandle,vShader);
    glAttachShader(programHandle,fShader);
    glLinkProgram(programHandle);

    //查询链接的结果  
    GLint linkStatus;  
    glGetProgramiv(programHandle,GL_LINK_STATUS,&linkStatus);  
    if(GL_FALSE == linkStatus)  
    {  
        printf("ERROR : link shader program failed");  
        GLint logLen;  
        glGetProgramiv(programHandle,GL_INFO_LOG_LENGTH, &logLen);  
        if (logLen > 0)  
        {  
            char *log = (char *)malloc(logLen);  
            GLsizei written;  
            glGetProgramInfoLog(programHandle,logLen, &written,log);  
            printf("Program log :%s\n", log);  
        }  
    }  
    else//链接成功，在OpenGL管线中使用渲染程序  
    {  
        glUseProgram(programHandle);  
    }  
}
void initVBO()  
{
	//axis vao
    glGenVertexArrays(1,&axisvao);
    glBindVertexArray(axisvao);

	//axis
    glGenBuffers(1, &axispositionhandle);
    glBindBuffer(GL_ARRAY_BUFFER, axispositionhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*6, axispositiondata, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //color
    glGenBuffers(1, &axisnormalhandle);
    glBindBuffer(GL_ARRAY_BUFFER, axisnormalhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*6, axisnormaldata, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //color
    glGenBuffers(1, &axiscolorhandle);
    glBindBuffer(GL_ARRAY_BUFFER, axiscolorhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*6, axiscolordata, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);




	//sample vao
    glGenVertexArrays(1,&samplevao);
    glBindVertexArray(samplevao);

    //sample position
    glGenBuffers(1, &samplepositionhandle);
    glBindBuffer(GL_ARRAY_BUFFER, samplepositionhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*8, samplepositiondata, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //sample common
    glGenBuffers(1, &samplenormalhandle);
    glBindBuffer(GL_ARRAY_BUFFER, samplenormalhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*8, samplenormaldata, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //sample color
    glGenBuffers(1, &samplecolorhandle);
    glBindBuffer(GL_ARRAY_BUFFER, samplecolorhandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*8, samplecolordata, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    //sample index
    glGenBuffers(1, &sampleindexhandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sampleindexhandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*4*6, sampleindexdata, GL_STATIC_DRAW);
}  




void vectornormalize(float* v)
{
	float norm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
}
void vectorcross(float* v, float* x)
{
	//a × b= [a2b3-a3b2,a3b1-a1b3, a1b2-a2b1]
	float t[3] = {v[0], v[1], v[2]};
	v[0] = t[1]*x[2] - t[2]*x[1];
	v[1] = t[2]*x[0] - t[0]*x[2];
	v[2] = t[0]*x[1] - t[1]*x[0];
}
void quaternionnormalize(float* p)
{
	float norm = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2] + p[3]*p[3]);
	p[0] /= norm;
	p[1] /= norm;
	p[2] /= norm;
	p[3] /= norm;
}
void quaternionrotate(float* v, float* q)
{
	//t = 2 * cross(q.xyz, v)
	//v' = v + q.w * t + cross(q.xyz, t)
	//float tx = 2*q[2]*v[2]
	float j[3];
	float k[3];
	j[0] = (q[1]*v[2]-q[2]*v[1]) * 2;
	j[1] = (q[2]*v[0]-q[0]*v[2]) * 2;
	j[2] = (q[0]*v[1]-q[1]*v[0]) * 2;

	k[0] = q[1]*j[2]-q[2]*j[1];
	k[1] = q[2]*j[0]-q[0]*j[2];
	k[2] = q[0]*j[1]-q[1]*j[0];

	v[0] += q[3]*j[0] + k[0];
	v[1] += q[3]*j[1] + k[1];
	v[2] += q[3]*j[2] + k[2];
}
void matrixmultiply(GLfloat* u, GLfloat* v)
{
	int j;
	float w[16];
	for(j=0;j<16;j++)w[j] = u[j];

	u[ 0] = w[ 0]*v[ 0] + w[ 1]*v[ 4] + w[ 2]*v[ 8] + w[ 3]*v[12];
	u[ 1] = w[ 0]*v[ 1] + w[ 1]*v[ 5] + w[ 2]*v[ 9] + w[ 3]*v[13];
	u[ 2] = w[ 0]*v[ 2] + w[ 1]*v[ 6] + w[ 2]*v[10] + w[ 3]*v[14];
	u[ 3] = w[ 0]*v[ 3] + w[ 1]*v[ 7] + w[ 2]*v[11] + w[ 3]*v[15];

	u[ 4] = w[ 4]*v[ 0] + w[ 5]*v[ 4] + w[ 6]*v[ 8] + w[ 7]*v[12];
	u[ 5] = w[ 4]*v[ 1] + w[ 5]*v[ 5] + w[ 6]*v[ 9] + w[ 7]*v[13];
	u[ 6] = w[ 4]*v[ 2] + w[ 5]*v[ 6] + w[ 6]*v[10] + w[ 7]*v[14];
	u[ 7] = w[ 4]*v[ 3] + w[ 5]*v[ 7] + w[ 6]*v[11] + w[ 7]*v[15];

	u[ 8] = w[ 8]*v[ 0] + w[ 9]*v[ 4] + w[10]*v[ 8] + w[11]*v[12];
	u[ 9] = w[ 8]*v[ 1] + w[ 9]*v[ 5] + w[10]*v[ 9] + w[11]*v[13];
	u[10] = w[ 8]*v[ 2] + w[ 9]*v[ 6] + w[10]*v[10] + w[11]*v[14];
	u[11] = w[ 8]*v[ 3] + w[ 9]*v[ 7] + w[10]*v[11] + w[11]*v[15];

	u[12] = w[12]*v[ 0] + w[13]*v[ 4] + w[14]*v[ 8] + w[15]*v[12];
	u[13] = w[12]*v[ 1] + w[13]*v[ 5] + w[14]*v[ 9] + w[15]*v[13];
	u[14] = w[12]*v[ 2] + w[13]*v[ 6] + w[14]*v[10] + w[15]*v[14];
	u[15] = w[12]*v[ 3] + w[13]*v[ 7] + w[14]*v[11] + w[15]*v[15];
}
void fixmodel()
{
	//matrix = movematrix * rotatematrix * scalematrix
}
void fixview()
{
	//a X b = [ay*bz - az*by, az*bx-ax*bz, ax*by-ay*bx]
	float norm;

	//Z = center - camera
	float nx = center[0] - camera[0];
	float ny = center[1] - camera[1];
	float nz = center[2] - camera[2];
	norm = sqrt(nx*nx + ny*ny + nz*nz);
	nx /= norm;
	ny /= norm;
	nz /= norm;

	//X = cross(Z, above)
	float ux = ny*above[2] - nz*above[1];
	float uy = nz*above[0] - nx*above[2];
	float uz = nx*above[1] - ny*above[0];
	norm = sqrt(ux*ux + uy*uy + uz*uz);
	ux /= norm;
	uy /= norm;
	uz /= norm;

	//above = cross(X, Z)
	float vx = uy*nz - uz*ny;
	float vy = uz*nx - ux*nz;
	float vz = ux*ny - uy*nx;

	viewmatrix[0] = ux;
	viewmatrix[1] = vx;
	viewmatrix[2] = -nx;
	viewmatrix[3] = 0.0f;

	viewmatrix[4] = uy;
	viewmatrix[5] = vy;
	viewmatrix[6] = -ny;
	viewmatrix[7] = 0.0f;

	viewmatrix[8] = uz;
	viewmatrix[9] = vz;
	viewmatrix[10] = -nz;
	viewmatrix[11] = 0.0f;

	viewmatrix[12] = -camera[0]*ux - camera[1]*uy - camera[2]*uz;
	viewmatrix[13] = -camera[0]*vx - camera[1]*vy - camera[2]*vz;
	viewmatrix[14] = camera[0]*nx + camera[1]*ny + camera[2]*nz;
	viewmatrix[15] = 1.0f;
}
void fixprojection()
{
/*
	cot45, 0, 0, 0,
	0, cot45, 0, 0,
	0, 0, (f+n)/(f-n), -1,
	0, 0, (2*f*n)/(f-n), 0
*/
	float w = (float)width;
	float h = (float)height;
	glViewport(0, 0, width, height);
	projmatrix[0] = h / w;
}
void fixmatrix()
{
	int x;
	GLfloat temp[4*4];

	fixmodel();
	fixview();
	fixprojection();

	for(x=0;x<16;x++)temp[x] = modelmatrix[x];
	matrixmultiply(temp, viewmatrix);
	matrixmultiply(temp, projmatrix);

	GLint pvmmatrix = glGetUniformLocation(programHandle, "modelviewproj");
	glUniformMatrix4fv(pvmmatrix, 1, GL_FALSE, temp);
}
void fixlight()
{
	GLfloat ambientcolor[3] = {0.5f, 0.5f, 0.5f};
	GLfloat lightcolor[3] = {1.0f, 1.0f, 1.0f};
	GLfloat lightposition[3] = {0.1f, 0.2f, 5.0f};

	GLint ac = glGetUniformLocation(programHandle, "ambientcolor");
	glUniform3fv(ac, 1, ambientcolor);

	GLint dc = glGetUniformLocation(programHandle, "lightcolor");
	glUniform3fv(dc, 1, lightcolor);

	GLint dp = glGetUniformLocation(programHandle, "lightposition");
	glUniform3fv(dp, 1, lightposition);

	GLint ep = glGetUniformLocation(programHandle, "eyeposition");
	glUniform3fv(ep, 1, camera);
}
void display()  
{
	//set
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//
	fixmatrix();
	fixlight();

	//axis
	glBindVertexArray(axisvao);
	glDrawArrays(GL_LINES, 0, 6);

	//shape
	glBindVertexArray(samplevao);
	glDrawElements(GL_QUADS, 4*6, GL_UNSIGNED_SHORT, 0);

	//write
	glFlush();
    glutSwapBuffers();
}
void keyboard(unsigned char key,int x,int y)  
{
	printf("%d\n",key);
	glutPostRedisplay();
}
void callback_reshape(int w, int h)
{
	width = w;
	height = h;
}
void callback_move(int x,int y)
{
	float t[3];
	float v[4];
	t[0] = 0.0;
	t[1] = 0.0;
	t[2] = 1.0;
	v[0] = camera[0];
	v[1] = camera[1];
	v[2] = camera[2];
	v[3] = 0.0;
	if(x>last_x)
	{
		camera[0] = v[0]*cos(0.05f) + v[1]*sin(0.05f);
		camera[1] = -v[0]*sin(0.05f) + v[1]*cos(0.05f);

		//camera_yaw += PI/90;
	}
	else if(x<last_x)
	{
		camera[0] = v[0]*cos(0.05f) - v[1]*sin(0.05f);
		camera[1] = v[0]*sin(0.05f) + v[1]*cos(0.05f);

		//camera_yaw -= PI/90;
	}

	if(y > last_y)
	{
		vectorcross(v, t);
		vectornormalize(v);

		v[0] *= sin(0.02f);
		v[1] *= sin(0.02f);
		v[2] *= sin(0.02f);
		v[3] = cos(0.02f);
		quaternionrotate(camera, v);
	}
	else if(y<last_y)
	{
		vectorcross(v, t);
		vectornormalize(v);

		v[0] *= sin(-0.02f);
		v[1] *= sin(-0.02f);
		v[2] *= sin(-0.02f);
		v[3] = cos(-0.02f);
		quaternionrotate(camera, v);
	}

	last_x = x;
	last_y = y;
	glutPostRedisplay();
}
void callback_mouse(int button, int state, int x, int y)
{
	float tx, ty, tz;
	if(state == GLUT_DOWN)
	{
		last_x = x;
		last_y = y;
	}
	if(state == GLUT_UP)
	{
		tx = camera[0];
		ty = camera[1];
		tz = camera[2];
		if(button == 3)	//wheel_up
		{
			camera[0] = 0.9*tx + 0.1*center[0];
			camera[1] = 0.9*ty + 0.1*center[1];
			camera[2] = 0.9*tz + 0.1*center[2];

			//camera_zoom *= 0.95;
			glutPostRedisplay();
		}
		else if(button == 4)	//wheel_down
		{
			camera[0] = 1.1*tx - 0.1*center[0];
			camera[1] = 1.1*ty - 0.1*center[1];
			camera[2] = 1.1*tz - 0.1*center[2];

			//camera_zoom *= 1.05263158;
			glutPostRedisplay();
		}
	}
}
int main(int argc,char** argv)  
{
	int err;
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(256, 256);
    glutCreateWindow("GLSL Test : Draw a triangle");

    //初始化glew扩展库  
    err = glewInit();
    if( GLEW_OK != err )printf("glewinit: %s\n", glewGetErrorString(err));  

	glViewport(0, 0, 512, 512);
	glEnable(GL_DEPTH_TEST);
    initShader();
    initVBO();  

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	glutReshapeFunc(callback_reshape);
	glutMouseFunc(callback_mouse);
	glutMotionFunc(callback_move); 
      
    glutMainLoop();

	glDeleteShader(vShader);
	glUseProgram(0);
    return 0;  
}


/*
GLuint vertex;
GLuint index;
GLfloat vbo[] = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0
};
GLuint ibo[] = {
	0,1,2
};




void callback_display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glViewport(0, 0, 512, 512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 200.0);

	//
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);

	//
	glBufferData(GL_ARRAY_BUFFER, 4*9, vbo, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*3, ibo, GL_STATIC_DRAW);

	//
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
    glEnableVertexAttribArray(0);

	//
	//glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
	//glBindVertexArray(0);

	//
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//
	glFlush();
	glutSwapBuffers();
}
int main(int argc, char** argv)
{
	int ret;

	//glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("42");

	//glew
	ret = glewInit();

	//
	glutDisplayFunc(callback_display);

	//
	glGenBuffers(1, &vertex);
	glGenBuffers(1, &index);

	//
	glutMainLoop();

	//
	glDeleteBuffers(1, &vertex);
	glDeleteBuffers(1, &index);

	return 0;
}
*/



/*
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>

GLuint VBO;
GLfloat Vertices[] = {
	1.0f, -1.0f, -10.0f,
	1.0f, 1.0f, -10.0f,
	3.0f, 1.0f, -10.0f,
	3.0f, -1.0f, -10.0f
};


int VBOSize = 4;


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glViewport(0, 0, 512, 512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 200.0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, VBOSize);
    glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glutSwapBuffers();
}


int main(int argc, char** argv)
{
	int ret;

	//glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("42");

	//glew
	ret = glewInit();

    glutDisplayFunc(RenderSceneCB);

 	glGenBuffers(1, &VBO);

    glutMainLoop();
    
    return 0;
}
*/