c:
	clang test.c -lvulkan -lglfw
index:
	clang index.c -lvulkan -lglfw
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
