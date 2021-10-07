SDK=/opt/vulkansdk/1.2.189.0/x86_64

maccolor:shader
	clang ../glfw.c ../vulkan.c color.c -lvulkan -lglfw
macdepth:shader
	clang ../glfw.c ../vulkan.c depth.c -lvulkan -lglfw

wincolor:shader
	gcc ../glfw.c ../vulkan.c color.c -lvulkan-1 -lglfw3 -lgdi32
windepth:shader
	gcc ../glfw.c ../vulkan.c depth.c -lvulkan-1 -lglfw3 -lgdi32

linuxcolor:linuxshader
	gcc ../glfw.c ../vulkan.c color.c -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxdepth:linuxshader
	gcc ../glfw.c ../vulkan.c depth.c -I${SDK}/include -L${SDK}/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

offscreencolor:shader
	clang ../file.c ../vulkan.c color.c -lvulkan
offscreendepth:shader
	clang ../file.c ../vulkan.c depth.c -lvulkan

shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
linuxshader:
	${SDK}/bin/glslc shader.vert -o shader.vert.spv
	${SDK}/bin/glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
