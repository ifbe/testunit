global _main

section .data

	query_string:		db	"Enter a character:  "
	query_string_len:	equ	$ - query_string
	out_string:		db	"You have input:  "
	out_string_len:		equ	$ - out_string

section .bss

	in_char:		resw 4

section .text

_main:
	; write stdout
	mov	rax, 0x2000004	 	; write
	mov	rdi, 1			; stdout
	lea	rsi, [rel query_string]	; out buf
	mov	rdx, query_string_len	; len
	syscall

	; read stdin
	mov	rax, 0x2000003		; read
	mov	rdi, 0			; stdin
	lea	rsi, [rel in_char]	; in buf
	mov	rdx, 2			; [0]=char,[1]='\n'
	syscall

	; write stdout
	mov	rax, 0x2000004		; write
	mov	rdi, 1			; stdout
	lea	rsi, [rel out_string]	; out buf
	mov	rdx, out_string_len	; len
	syscall

	mov	rax, 0x2000004		; write
	mov	rdi, 1			; stdout
	lea	rsi, [rel in_char]	; out buf
	mov	rdx, 2			; len
	syscall

	; exit system call
	mov	rax, 0x2000001		; exit
	xor     rdi, rdi
	syscall
