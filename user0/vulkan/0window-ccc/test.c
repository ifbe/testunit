#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};




//0
static void* glfw;	//for fun
static VkInstance instance;
//1
static GLFWwindow* window = 0;
VkSurfaceKHR surface;




int freeglfw()
{
	glfwTerminate();
	return 0;
}
int initglfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return 0;
}




int freeinstance()
{
	vkDestroyInstance(instance, 0);
	return 0;
}
int initinstance()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, 0);

	VkLayerProperties availableLayers[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	printf("vkEnumerateInstanceLayerProperties:\n");

	int j;
	for(j=0;j<layerCount;j++){
		printf("%4d:%s\n", j, availableLayers[j].layerName);
	}




	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t count = 0;
	const char** extension = glfwGetRequiredInstanceExtensions(&count);
	printf("glfwGetRequiredInstanceExtensions:\n");

	for(j=0;j<count;j++){
		printf("%4d:%s\n", j, extension[j]);
	}

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = count;
	createInfo.ppEnabledExtensionNames = extension;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = 0;

	if (vkCreateInstance(&createInfo, 0, &instance) != VK_SUCCESS) {
		printf("failed to create instance!");
	}	
	return 0;
}




int freeglfwwindow()
{
	glfwDestroyWindow(window);
	return 0;
}
int initglfwwindow()
{
	window = glfwCreateWindow(1024, 768, "Vulkan window", 0, 0);
	if(0 == window){
		printf("error@glfwCreateWindow\n");
		return -1;
	}
	return 0;
}




int freesurface()
{
	vkDestroySurfaceKHR(instance, surface, 0);
	return 0;
}
int initsurface()
{
	if (glfwCreateWindowSurface(instance, window, 0, &surface) != VK_SUCCESS) {
		printf("error@glfwCreateWindowSurface\n");
		return -1;
	}
	return 0;
}




int main()
{
	//glfw, vulkan
	initglfw();
	initinstance();

	//window, surface
	initglfwwindow();
	initsurface();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	freesurface();
	freeglfwwindow();

	freeinstance();
	freeglfw();
	return 0;
}
