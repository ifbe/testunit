vertex:shader
	clang test.c -lvulkan -lglfw
indice:shader
	clang index.c -lvulkan -lglfw
winvertex:shader
	gcc test.c -lvulkan-1 -lglfw3 -lgdi32
winindice:shader
	gcc index.c -lvulkan-1 -lglfw3 -lgdi32
linuxvertex:shader
	gcc test.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxindice:shader
	gcc index.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
