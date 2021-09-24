color:shader
	clang ../main.c color.c -lvulkan -lglfw
depth:shader
	clang ../main.c depth.c -lvulkan -lglfw
wincolor:shader
	gcc ../main.c color.c -lvulkan-1 -lglfw3 -lgdi32
windepth:shader
	gcc ../main.c depth.c -lvulkan-1 -lglfw3 -lgdi32
linuxcolor:shader
	gcc ../main.c color.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxdepth:shader
	gcc ../main.c depth.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
