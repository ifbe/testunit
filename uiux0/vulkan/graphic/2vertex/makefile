vertex:shader
	clang ../glfw.c ../vulkan.c vertex.c -lvulkan -lglfw
indice:shader
	clang ../glfw.c ../vulkan.c indice.c -lvulkan -lglfw
winvertex:shader
	gcc ../glfw.c ../vulkan.c vertex.c -lvulkan-1 -lglfw3 -lgdi32
winindice:shader
	gcc ../glfw.c ../vulkan.c indice.c -lvulkan-1 -lglfw3 -lgdi32
linuxvertex:shader
	gcc ../glfw.c ../vulkan.c vertex.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
linuxindice:shader
	gcc ../glfw.c ../vulkan.c indice.c -I/opt/vulkan/x86_64/include -L/opt/vulkan/x86_64/lib -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
