LINUXSDK=/opt/vulkansdk/1.3.268.0/x86_64
MACSDK=~/VulkanSDK/1.3.268.1/macOS
WINSDK=C:\VulkanSDK\1.3.268.0

glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

macvertex:shader
	clang vertex.c ${glfwvulkan} -lvulkan -lglfw
	install_name_tool -add_rpath ${MACSDK}/lib a.out
macindice:shader
	clang indice.c ${glfwvulkan} -lvulkan -lglfw
	install_name_tool -add_rpath ${MACSDK}/lib a.out
winvertex:shader
	gcc vertex.c ${glfwvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32
winindice:shader
	gcc indice.c ${glfwvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32
linuxvertex:shader
	gcc vertex.c ${glfwvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxindice:shader
	gcc indice.c ${glfwvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
