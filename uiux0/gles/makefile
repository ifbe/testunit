linux:
	gcc graphic.c -lGLESv2 -lEGL
linuxcompute:
	gcc compute.c -lGLESv2 -lEGL -lgbm
macos:
	gcc graphic.c -I/Users/ifbe/Downloads/MoltenGL-0.29.0/Demos/Common/API -L /Users/ifbe/Downloads/MoltenGL-0.29.0/dylib/macOS/ -lMoltenGL -framework OpenGL
run:
	DISPLAY=:0 ./a.out
