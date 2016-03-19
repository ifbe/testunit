.PHONY:bare
binary:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/screen.c
	gcc -c -fPIC -O2 bare/input.c
	ld -T bare/link.ld -o ../color.bin \
	start.o \
	main/main.a \
	wrap1/wrap1.a \
	screen.o input.o
.PHONY:cross
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	rm -f wrap.a
	ar crs wrap.a screen.o input.o
.PHONY:linux
linux:
	gcc -c -fPIC -O2 system/screen.c
	gcc -c -fPIC -O2 system/sdl.c
	rm -f wrap.a
	ar crs wrap.a screen.o sdl.o
.PHONY:mac
mac:
	clang -c -fPIC -O2 system/screen.c
	clang -c -fPIC -O2 system/sdl.c -I /usr/local/Cellar/sdl2/2.0.3/include
	rm -f wrap.a
	ar crs wrap.a screen.o sdl.o
.PHONY:win+api
win+api:
	gcc -c -O2 win/api.c
	gcc -mwindows -o ../a.exe \
	../main/main.a \
	../wrap1/wrap1.a \
	api.o \
	-lgdi32
.PHONY:win+sdl
win+sdl:
	gcc -c -O2 sdl/sdl.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -mwindows -o ../a.exe \
	../main/main.a \
	../wrap1/wrap1.a \
	sdl.o \
	-Wl,--no-undefined -static -static-libgcc \
	-L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	-lmingw32 -lSDL2main -lSDL2 \
	-lm -ldinput8 -ldxguid -ldxerr8 \
	-luser32 -lgdi32 -lwinmm -limm32 \
	-lole32 -loleaut32 -lshell32 \
	-lversion -luuid