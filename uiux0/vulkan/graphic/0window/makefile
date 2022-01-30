SDK=/opt/vulkansdk/1.2.189.0/x86_64

mac:
	clang ../glfw.c ../vulkan.c window.c -lvulkan -lglfw
win:
	gcc ../glfw.c ../vulkan.c window.c -lvulkan-1 -lglfw3 -lgdi32
rpi:
	gcc ../glfw.c ../vulkan.c window.c -lvulkan -lglfw
linux:
	gcc ../glfw.c ../vulkan.c window.c -I${SDK}/include/ -L${SDK}/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread
offscreen:
	gcc ../file.c ../vulkan.c window.c -lvulkan
clean:
	rm *.out *.spv
