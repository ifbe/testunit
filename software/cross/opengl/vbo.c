#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define PI 3.1415926535897932384626433832795028841971693993151

GLuint vShader;
GLuint fShader;
GLuint programHandle;

GLuint vaoHandle;
GLuint position;
GLuint color;
GLuint index;

float camerax = 0.0f;
float cameray = 2.0f;
float cameraz = 0.0f;
float centerx = 0.0f;
float centery = 0.0f;
float centerz = 0.0f;
float abovex = 0.0f;
float abovey = 0.0f;
float abovez = 1.0f;

float angle = 0;
static int last_x=0;
static int last_y=0;

float positionData[] = {
	-0.25, -0.25, -0.5,
	0.25, -0.25, -0.5,
	0.25, 0.25, -0.5,
	-0.25, 0.25, -0.5,
	-0.5, -0.5, 0.25,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.75,
	-0.5, 0.5, 1.0,
};
/*
float colorData[] = {  
	1.0f, 0.0f, 0.0f,  
	0.0f, 1.0f, 0.0f,  
	0.0f, 0.0f, 1.0f
};
*/
unsigned short indexdata[] = {
	0, 1, 2, 3,
	0, 1, 5, 4,
	5, 6, 2, 1,
	5, 6, 7, 4,
	3, 7, 4, 0,
	3, 7, 6, 2
};
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




char vCode[] = {
	"#version 400\n"
	"in vec3 position;\n"
	"out vec3 vertexcolor;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 proj;\n"
	"void main()\n"
	"{\n"
		"vertexcolor = position;\n"
		//"gl_Position = vec4(position,1.0);\n"
		"gl_Position = proj * view * model * vec4(position,1.0);\n"
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
	//vao
    glGenVertexArrays(1,&vaoHandle);
    glBindVertexArray(vaoHandle);

    //position
    glGenBuffers(1, &position);
    glBindBuffer(GL_ARRAY_BUFFER, position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*8, positionData, GL_STATIC_DRAW);
/*
    //color
    glGenBuffers(1, &color);
    glBindBuffer(GL_COLOR_BUFFER, color);
    glBufferData(GL_COLOR_BUFFER, 9*sizeof(float), colorData, GL_STATIC_DRAW);

    //common
    glGenBuffers(1, &common);
    glBindBuffer(GL_ARRAY_BUFFER, common);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), colorData, GL_STATIC_DRAW);
*/
    //index
    glGenBuffers(1, &index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*4*6, indexdata, GL_STATIC_DRAW);

	//顶点坐标
    glBindBuffer(GL_ARRAY_BUFFER, position);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
/*
	//顶点颜色
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//顶点纹理
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
*/
}  




void fixmodel()
{
	//matrix = movematrix * rotatematrix * scalematrix
	GLint modelLoc = glGetUniformLocation(programHandle, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelmatrix);
}
void fixview()
{
	//a X b = [ay*bz - az*by, az*bx-ax*bz, ax*by-ay*bx]
	float norm;

	//Z = center - camera
	float nx = centerx - camerax;
	float ny = centery - cameray;
	float nz = centerz - cameraz;
	norm = sqrt(nx*nx + ny*ny + nz*nz);
	nx /= norm;
	ny /= norm;
	nz /= norm;

	//X = cross(Z, above)
	float ux = ny*abovez - nz*abovey;
	float uy = nz*abovex - nx*abovez;
	float uz = nx*abovey - ny*abovex;
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

	viewmatrix[12] = -camerax*ux - cameray*uy - cameraz*uz;
	viewmatrix[13] = -camerax*vx - cameray*vy - cameraz*vz;
	viewmatrix[14] = camerax*nx + cameray*ny + cameraz*nz;
	viewmatrix[15] = 1.0f;
/*
	viewmatrix[0] = cos(camerax);
	viewmatrix[2] = -sin(camerax);
	viewmatrix[8] = sin(camerax);
	viewmatrix[10] = cos(camerax);
	viewmatrix[14] = -1.0f;
*/
	GLint viewLoc = glGetUniformLocation(programHandle, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewmatrix);
}
void fixprojection()
{
/*
	cot45, 0, 0, 0,
	0, cot45, 0, 0,
	0, 0, (f+n)/(f-n), -1,
	0, 0, (2*f*n)/(f-n), 0
*/
	GLint projLoc = glGetUniformLocation(programHandle, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projmatrix);
}
void display()  
{
	//set
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//draw
	glBindVertexArray(vaoHandle);
	glDrawElements(GL_QUADS, 4*6, GL_UNSIGNED_SHORT, 0);

	//matrix
	fixmodel();
	fixview();
	fixprojection();

	//write
	glFlush();
    glutSwapBuffers();
}
void keyboard(unsigned char key,int x,int y)  
{
	printf("%d\n",key);
	glutPostRedisplay();
}
void callback_move(int x,int y)
{
	float tx = camerax;
	float ty = cameray;
	if(x>last_x)
	{
		camerax = tx*cos(0.1f) + ty*sin(0.1f);
		cameray = -tx*sin(0.1f) + ty*cos(0.1f);

		//camera_yaw += PI/20;
	}
	else if(x<last_x)
	{
		camerax = tx*cos(0.1f) - ty*sin(0.1f);
		cameray = tx*sin(0.1f) + ty*cos(0.1f);

		//camera_yaw -= PI/20;
	}

	if(y>last_y)
	{
		cameraz += 0.1;
		//cameray += 0.1f;
		//if(camera_pitch < PI*44/90)camera_pitch += PI/90;
	}
	else if(y<last_y)
	{
		cameraz -= 0.1;
		//cameray -= 0.1f;
		//if(camera_pitch > -PI*44/90)camera_pitch -= PI/90;
	}

	last_x = x;
	last_y = y;
	glutPostRedisplay();
}
void callback_mouse(int button, int state, int x, int y)
{
	float tx = camerax;
	float ty = cameray;
	float tz = cameraz;
	if(state == GLUT_DOWN)
	{
		last_x = x;
		last_y = y;
	}
	if(state == GLUT_UP)
	{
		if(button == 3)	//wheel_up
		{
			camerax = 0.9*tx + 0.1*centerx;
			cameray = 0.9*ty + 0.1*centery;
			cameraz = 0.9*tz + 0.1*centerz;
/*
			//camera_zoom *= 0.95;
			viewmatrix[14] += 0.1f;
*/
			glutPostRedisplay();
		}
		if(button == 4)	//wheel_down
		{
			camerax = 1.1*tx - 0.1*centerx;
			cameray = 1.1*ty - 0.1*centery;
			cameraz = 1.1*tz - 0.1*centerz;
			glutPostRedisplay();
/*
			viewmatrix[14] -= 0.1f;
			//camera_zoom *= 1.05263158;
*/
		}
		//printf("camera_zoom=%f\n",camera_zoom);
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

	glViewport(-256, -256, 512, 512);
	glEnable(GL_DEPTH_TEST);
    initShader();
    initVBO();  

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyboard);
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