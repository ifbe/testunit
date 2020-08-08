test:shader
	clang++ -std=c++2a test.cpp -lvulkan -lglfw -lm
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
