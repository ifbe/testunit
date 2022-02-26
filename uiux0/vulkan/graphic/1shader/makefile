WINSDK=C:\VulkanSDK\1.3.204.0
LINUXSDK=/opt/vulkansdk/1.3.204.0/x86_64
sdlvulkan=../../main_sdl.c ../../vulkan.c
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

macglfwvulkancolor:
	clang color.c ${glfwvulkan} -lvulkan -lglfw
macglfwvulkandepth:
	clang depth.c ${glfwvulkan} -lvulkan -lglfw
macsdlvulkancolor:
	clang color.c ${sdlvulkan} -lvulkan -lglfw
macsdlvulkandepth:
	clang depth.c ${sdlvulkan} -lvulkan -lglfw

winglfwvulkancolor:
	gcc color.c ${glfwvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32
winglfwvulkandepth:
	gcc depth.c ${glfwvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32
winsdlvulkancolor:
	gcc color.c ${sdlvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32
winsdlvulkandepth:
	gcc depth.c ${sdlvulkan} -I${WINSDK}/Include -L${WINSDK}/Lib -lvulkan-1 -lglfw3 -lgdi32

linuxglfwvulkancolor:
	gcc color.c ${glfwvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxglfwvulkandepth:
	gcc depth.c ${glfwvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxsdlvulkancolor:
	gcc color.c ${sdlvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxsdlvulkandepth:
	gcc depth.c ${sdlvulkan} -I${LINUXSDK}/include -L${LINUXSDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

stdoutcolor:
	gcc color.c ${stdoutvulkan} -lvulkan
stdoutdepth:
	gcc depth.c ${stdoutvulkan} -lvulkan

filecolor:
	gcc color.c ${filevulkan} -lvulkan
filedepth:
	gcc depth.c ${filevulkan} -lvulkan

glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.vert -o shader.vert.spv
	glslangValidator --target-env vulkan1.2 shader.frag -o shader.frag.spv
glslc:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
linuxglslc:
	${LINUXSDK}/bin/glslc shader.vert -o shader.vert.spv
	${LINUXSDK}/bin/glslc shader.frag -o shader.frag.spv
winglslc:
	${WINSDK}/bin/glslc shader.vert -o shader.vert.spv
	${WINSDK}/bin/glslc shader.frag -o shader.frag.spv

offscreenrun:
	DISPLAY=:0 ./a.out

clean:
	rm *.out *.spv
