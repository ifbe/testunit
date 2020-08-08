color:shader
	clang test.c -lvulkan -lglfw
depth:shader
	clang depth.c -lvulkan -lglfw
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
