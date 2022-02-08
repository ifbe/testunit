linux:
	gcc compute.c -lGLESv2 -lEGL
macos:
	gcc compute.c -I/Users/ifbe/Downloads/MoltenGL-0.29.0/Demos/Common/API -L /Users/ifbe/Downloads/MoltenGL-0.29.0/dylib/macOS/ -lMoltenGL -framework OpenGL
run:
	DISPLAY=:0 ./a.out
