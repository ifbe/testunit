vertex:shader
	clang ../main.c vertex.c -lvulkan -lglfw
indice:shader
	clang ../main.c indice.c -lvulkan -lglfw
winvertex:shader
	gcc ../main.c vertex.c -lvulkan-1 -lglfw3 -lgdi32
winindice:shader
	gcc ../main.c indice.c -lvulkan-1 -lglfw3 -lgdi32
linuxvertex:shader
	gcc ../main.c vertex.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxindice:shader
	gcc ../main.c indice.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
