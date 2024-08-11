LINUXSDK=/opt/vulkansdk/1.3.268.0/x86_64
MACSDK=~/VulkanSDK/1.3.268.1/macOS
WINSDK=C:\VulkanSDK\1.3.268.0
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

macglfwvulkan:
	clang window.c ${glfwvulkan} -lvulkan -lglfw
	install_name_tool -add_rpath ${MACSDK}/lib a.out

winglfwvulkan:
	gcc window.c ${glfwvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32

rpiglfwvulkan:
	gcc window.c ${glfwvulkan} -lvulkan -lglfw

linuxglfwvulkan:
	gcc window.c ${glfwvulkan} -I${LINUXSDK}/include/ -L${LINUXSDK}/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

offscreenvulkan:
	gcc window.c ${stdoutvulkan} -lvulkan

clean:
	rm *.out *.spv
