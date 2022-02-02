linux:
	gcc test.c -lGLESv2 -lEGL
linuxgbm:
	gcc gbm.c -lGLESv2 -lEGL -lgbm
macos:
	gcc test.c -I/Users/ifbe/Downloads/MoltenGL-0.29.0/Demos/Common/API -L /Users/ifbe/Downloads/MoltenGL-0.29.0/dylib/macOS/ -lMoltenGL
run:
	DISPLAY=:0 ./a.out
