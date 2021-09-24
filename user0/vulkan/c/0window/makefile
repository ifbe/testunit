mac:
	clang ../main.c vk.c -lvulkan -lglfw
win:
	gcc ../main.c vk.c -lvulkan-1 -lglfw3 -lgdi32
linux:
	gcc ../main.c vk.c -I /opt/vulkan/x86_64/include/ -L /opt/vulkan/x86_64/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
clean:
	rm *.out *.spv
