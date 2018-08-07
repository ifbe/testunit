#include<stdio.h>
#include<stdlib.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
int main()
{
	if(glfwInit() == 0)
	{
		printf("error@glfwInit\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);	//4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	//MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(1024, 768, "test", NULL, NULL);
	if(win == NULL)
	{
		printf("error@glfwCreateWindow\n");
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(win);
	glewExperimental = 1;

	if(glewInit() != GLEW_OK)
	{
		printf("error@glewInit\n");
		return -3;
	}

	while(1)
	{
		if(glfwWindowShouldClose(win) != 0)break;
		if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)break;

		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	return 0;
}
