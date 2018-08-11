global _main

section .data
msg:	db	"Hello, world!", 10
len:	equ	$ - msg

section .text
_main:
	mov eax, 4		; write
	mov ebx, 1		; stdout
	lea ecx, [rel msg]
	mov edx, len
	int 0x80

	mov eax, 1	; exit
	mov ebx, 0	; code
	int 0x80
