binary:
	gcc -c -fPIC -O2 color.c
	rm -f main.a
	ar crs main.a color.o
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 color.c
	rm -f main.a
	ar crs main.a color.o
linux:
	gcc -c -fPIC -O2 color.c
	rm -f main.a
	ar crs main.a color.o
mac:
	clang -c -fPIC -O2 color.c
	rm -f main.a
	ar crs main.a color.o
win:
	gcc -c -O2 color.c
	rm -f main.a
	ar crs main.a color.o