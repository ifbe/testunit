cpp:
	clang++ -std=c++2a test.cpp -lvulkan -lglfw
index:
	clang++ -std=c++2a index.cpp -lvulkan -lglfw
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
