SDK=/opt/vulkansdk/1.2.189.0/x86_64
glfwvulkan=../../main_glfw.c ../../vulkan.c
filevulkan=../../main_file.c ../../vulkan.c
stdoutvulkan=../../main_stdout.c ../../vulkan.c

offscreenvulkan:
	gcc test.c ${stdoutvulkan} -lvulkan

glslc:
	glslc shader.comp -o shader.comp.spv
glslangvalidator:
	glslangValidator --target-env vulkan1.2 shader.comp -o shader.comp.spv

clean:
	rm *.out *.spv
