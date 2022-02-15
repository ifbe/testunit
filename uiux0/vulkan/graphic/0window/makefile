SDK=/opt/vulkansdk/1.2.189.0/x86_64
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

macglfwvulkan:
	clang window.c ${glfwvulkan} -lvulkan -lglfw

winglfwvulkan:
	gcc window.c ${glfwvulkan} -lvulkan-1 -lglfw3 -lgdi32

rpiglfwvulkan:
	gcc window.c ${glfwvulkan} -lvulkan -lglfw

linuxglfwvulkan:
	gcc window.c ${glfwvulkan} -I${SDK}/include/ -L${SDK}/lib/ -lvulkan -lglfw3 -lX11 -ldl -lm -lpthread

offscreenvulkan:
	gcc window.c ${stdoutvulkan} -lvulkan

clean:
	rm *.out *.spv
