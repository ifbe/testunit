binary:
	gcc -c -fPIC -O2 zimo.c
	rm -f main.a
	ar crs main.a zimo.o
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 zimo.c
	rm -f main.a
	ar crs main.a zimo.o
linux:
	gcc -c -fPIC -O2 zimo.c
	rm -f main.a
	ar crs main.a zimo.o
mac:
	clang -c -fPIC -O2 zimo.c
	rm -f main.a
	ar crs main.a zimo.o
win:
	gcc -c -O2 zimo.c
	rm -f main.a
	ar crs main.a zimo.o