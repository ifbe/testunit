color:shader
	clang test.c -lvulkan -lglfw
depth:shader
	clang depth.c -lvulkan -lglfw
wincolor:shader
	gcc test.c -lvulkan-1 -lglfw3 -lgdi32
windepth:shader
	gcc depth.c -lvulkan-1 -lglfw3 -lgdi32
shader:
	glslc shader.vert -o shader.vert.spv
	glslc shader.frag -o shader.frag.spv
clean:
	rm *.out *.spv
