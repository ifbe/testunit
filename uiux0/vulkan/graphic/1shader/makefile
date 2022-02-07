SDK=/opt/vulkansdk/1.2.198.1/x86_64

maccolor:
	clang ../glfw.c ../vulkan.c color.c -lvulkan -lglfw
macdepth:
	clang ../glfw.c ../vulkan.c depth.c -lvulkan -lglfw

wincolor:
	gcc ../glfw.c ../vulkan.c color.c -lvulkan-1 -lglfw3 -lgdi32
windepth:
	gcc ../glfw.c ../vulkan.c depth.c -lvulkan-1 -lglfw3 -lgdi32

linuxcolor:
	gcc ../glfw.c ../vulkan.c color.c -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxdepth:
	gcc ../glfw.c ../vulkan.c depth.c -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

stdoutcolor:
	gcc ../stdout.c ../vulkan.c color.c -lvulkan
stdoutdepth:
	gcc ../stdout.c ../vulkan.c depth.c -lvulkan

offscreencolor:
	gcc ../file.c ../vulkan.c color.c -lvulkan
offscreendepth:
	gcc ../file.c ../vulkan.c depth.c -lvulkan

offscreenrun:
	DISPLAY=:0 ./a.out

glslc:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
sdkglslc:
	${SDK}/bin/glslc shader.vert -o shader.vert.spv
	${SDK}/bin/glslc shader.frag -o shader.frag.spv
glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.vert -o shader.vert.spv
	glslangValidator --target-env vulkan1.2 shader.frag -o shader.frag.spv

clean:
	rm *.out *.spv
