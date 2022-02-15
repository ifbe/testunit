SDK=/opt/vulkansdk/1.2.198.1/x86_64
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
macsdlkandepth:
	clang depth.c ${sdlvulkan} -lvulkan -lglfw

winglfwvulkancolor:
	gcc ../glfw.c ../vulkan.c color.c ${glfwvulkan} -lvulkan-1 -lglfw3 -lgdi32
winglfwvulkandepth:
	gcc ../glfw.c ../vulkan.c depth.c ${glfwulkan}} -lvulkan-1 -lglfw3 -lgdi32
winsdlvulkancolor:
	gcc ../glfw.c ../vulkan.c color.c ${sdlvulkan} -lvulkan-1 -lglfw3 -lgdi32
winsdlvulkandepth:
	gcc ../glfw.c ../vulkan.c depth.c ${sdlvulkan}} -lvulkan-1 -lglfw3 -lgdi32

linuxglfwvulkancolor:
	gcc ../glfw.c ../vulkan.c color.c ${glfwulkan}} -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxglfwvulkandepth:
	gcc ../glfw.c ../vulkan.c depth.c ${glfwulkan}} -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxsdlvulkancolor:
	gcc ../glfw.c ../vulkan.c color.c ${sdlvulkan}} -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxsdlvulkandepth:
	gcc ../glfw.c ../vulkan.c depth.c ${sdlvulkan}} -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

stdoutcolor:
	gcc ../stdout.c ../vulkan.c color.c ${stdoutvulkan} -lvulkan
stdoutdepth:
	gcc ../stdout.c ../vulkan.c depth.c ${stdoutvulkan} -lvulkan

filecolor:
	gcc ../file.c ../vulkan.c color.c ${filevulkan} -lvulkan
filedepth:
	gcc ../file.c ../vulkan.c depth.c ${filevulkan} -lvulkan

glslc:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
sdkglslc:
	${SDK}/bin/glslc shader.vert -o shader.vert.spv
	${SDK}/bin/glslc shader.frag -o shader.frag.spv
glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.vert -o shader.vert.spv
	glslangValidator --target-env vulkan1.2 shader.frag -o shader.frag.spv

offscreenrun:
	DISPLAY=:0 ./a.out

clean:
	rm *.out *.spv
