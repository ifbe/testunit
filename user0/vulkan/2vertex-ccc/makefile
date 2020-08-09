vertex:shader
	clang test.c -lvulkan -lglfw
indice:shader
	clang index.c -lvulkan -lglfw
winvertex:shader
	gcc test.c -lvulkan-1 -lglfw3 -lgdi32
winindice:shader
	gcc index.c -lvulkan-1 -lglfw3 -lgdi32
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
