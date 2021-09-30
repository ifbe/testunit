mac:shader
	clang++ -std=c++2a test.cpp -lvulkan -lglfw
win:shader
	g++ -std=c++2a test.cpp -lvulkan-1 -lglfw3 -lgdi32
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
