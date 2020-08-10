color:shader
	clang test.c -lvulkan -lglfw
depth:shader
	clang depth.c -lvulkan -lglfw
wincolor:shader
	gcc test.c -lvulkan-1 -lglfw3 -lgdi32
windepth:shader
	gcc depth.c -lvulkan-1 -lglfw3 -lgdi32
linuxcolor:shader
	gcc test.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxdepth:shader
	gcc depth.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
