all:
	gcc -c -O2 -fPIC -ffreestanding start.c
	gcc -c -O2 -fPIC -ffreestanding main.c
	ld -T link.ld start.o main.o
