SDK=/opt/vulkansdk/1.2.189.0/x86_64

mac:
	clang ../glfw.c ../vulkan.c vk.c -lvulkan -lglfw
win:
	gcc ../glfw.c ../vulkan.c vk.c -lvulkan-1 -lglfw3 -lgdi32
linux:
	gcc ../glfw.c ../vulkan.c vk.c -I${SDK}/include/ -L${SDK}/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
offscreen:
	gcc ../file.c ../vulkan.c vk.c -lvulkan
clean:
	rm *.out *.spv
