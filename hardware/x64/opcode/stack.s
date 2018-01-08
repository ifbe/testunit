[bits 64]

push rax	;50
push rcx	;51
push rdx	;52
push rbx	;53
push rsp	;54
push rbp	;55
push rsi	;56
push rdi	;57

pop rax		;58
pop rcx		;59
pop rdx		;5a
pop rbx		;5b
pop rsp		;5c
pop rbp		;5d
pop rsi		;5e
pop rdi		;5f

call rax	;ffd0
call rcx	;ffd1
call rdx	;ffd2
call rbx	;ffd3
call rsp	;ffd4
call rbp	;ffd5
call rsi	;ffd6
call rdi	;ffd7

jmp rax		;ffe0
jmp rcx		;ffe1
jmp rdx		;ffe2
jmp rbx		;ffe3
jmp rsp		;ffe4
jmp rbp		;ffe5
jmp rsi		;ffe6
jmp rdi		;ffe7

ret			;c3
iretq		;48cf