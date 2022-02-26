LINUXSDK=/opt/vulkansdk/1.3.204.0/x86_64
WINSDK=C:\VulkanSDK\1.3.204.0
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

offscreenvulkan:
	gcc test.c ${stdoutvulkan} -lvulkan

clean:
	rm *.out *.spv
