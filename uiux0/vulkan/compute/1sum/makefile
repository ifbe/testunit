LINUXSDK=/opt/vulkansdk/1.3.250.1/x86_64
WINSDK=C:\VulkanSDK\1.3.250.1
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

offscreenwin:
	gcc test.c ${stdoutvulkan} -lvulkan-1 -I${WINSDK}/Include -L${WINSDK}/Lib
offscreenmac:
	gcc test.c ${stdoutvulkan} -lvulkan

glslc:
	${WINSDK}\Bin\glslc.exe shader.comp -o shader.comp.spv
glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.comp -o shader.comp.spv

clean:
	rm *.out *.spv
