#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void* vulkan_init(int cnt, const char** ext);
void vulkan_exit();
void vulkan_device_create(void*);
void vulkan_device_delete();
void vulkan_myctx_create();
void vulkan_myctx_delete();
void drawframe();




int glfw_exit()
{
	glfwTerminate();
	return 0;
}
int glfw_init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return 0;
}




int glfw_window_delete(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	return 0;
}
void* glfw_window_create()
{
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Vulkan window", 0, 0);
	if(0 == window){
		printf("error@glfwCreateWindow\n");
		return 0;
	}

	return window;
}




int glfw_surface_delete()
{
	return 0;
}
void* glfw_surface_create(void* window, void* instance)
{
	void* surface;
	if (glfwCreateWindowSurface(instance, window, 0, (void*)&surface) != VK_SUCCESS) {
		printf("error@glfwCreateWindowSurface\n");
		return 0;
	}
	return surface;
}




int main()
{
	//init
	glfw_init();

	//vulkan
	uint32_t j,count = 0;
	const char** extension = glfwGetRequiredInstanceExtensions(&count);
	printf("glfwGetRequiredInstanceExtensions:\n");
	for(j=0;j<count;j++){
		printf("%4d:%s\n", j, extension[j]);
	}
	void* instance = vulkan_init(count, extension);

	//glfw: window
	GLFWwindow* window = glfw_window_create();

	//glfw: surface
	void* surface = glfw_surface_create(window, instance);

	//vulkan: device and swapchain
	vulkan_device_create(surface);

	//vulkan: things
	vulkan_myctx_create();

	//forever
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawframe();
	}

	//vulkan
	vulkan_myctx_delete();
	vulkan_device_delete();

	//glfw
	glfw_surface_delete();
	glfw_window_delete(window);

	//exit
	vulkan_exit();
	glfw_exit();
	return 0;
}
