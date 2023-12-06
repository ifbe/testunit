LINUXSDK=/opt/vulkansdk/1.3.261.1/x86_64
MACSDK=~/VulkanSDK/1.3.261.1/macOS
WINSDK=C:\VulkanSDK\1.3.261.1
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

offscreen:
	gcc test.c ${stdoutvulkan} -lvulkan
offscreenlinux:
	gcc test.c ${stdoutvulkan} -lvulkan -I${LINUXSDK}/include -L${LINUXSDK}/lib
offscreenwin:
	gcc test.c ${stdoutvulkan} -lvulkan-1 -I${WINSDK}/Include -L${WINSDK}/Lib

glslc:
	${WINSDK}\Bin\glslc.exe shader.comp -o shader.comp.spv
glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.comp -o shader.comp.spv

clean:
	rm *.out *.spv
