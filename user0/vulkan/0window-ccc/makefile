mac:
	clang test.c -lvulkan -lglfw
win:
	gcc test.c -lvulkan-1 -lglfw3 -lgdi32
linux:
	gcc test.c -I /opt/vulkan/x86_64/include/ -L /opt/vulkan/x86_64/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
clean:
	rm *.out *.spv
