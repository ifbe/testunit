LINUXSDK=/opt/vulkansdk/1.3.268.0/x86_64
MACSDK=~/VulkanSDK/1.3.268.1/macOS
WINSDK=C:\VulkanSDK\1.3.268.0

glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

offscreen:
	gcc test.c ${stdoutvulkan} -lvulkan
offscreen-linux:
	gcc test.c ${stdoutvulkan} -lvulkan -I${LINUXSDK}/include -L${LINUXSDK}/lib
offscreen-win:
	gcc test.c ${stdoutvulkan} -lvulkan-1 -I${WINSDK}/Include -L${WINSDK}/Lib
offscreen-mac:offscreen
	install_name_tool -add_rpath ${MACSDK}/lib a.out

clean:
	rm *.out *.spv
