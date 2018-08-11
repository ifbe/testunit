win:
	yasm -f win64 -o win.o win.asm
	gcc win.o
mac:
	yasm -f macho64 -o mac.o mac.asm
	gcc mac.o
linux:
	yasm -f elf64 -o linux.o linux.asm
	clang linux.o
clean:
	rm *.o *.out
