[bits 64]
mov spl,al		;4088c4
mov bpl,al		;4088c5
mov sil,al		;4088c6
mov dil,al		;4088c7

mov al,ah		;88e0
mov cl,ah		;88e1
mov dl,ah		;88e2
mov bl,ah		;88e3
mov ah,ah		;88e4
mov ch,ah		;88e5
mov dh,ah		;88e6
mov bh,ah		;88e7

mov al,al		;88c0
mov cl,al		;88c1
mov dl,al		;88c2
mov bl,al		;88c3
mov ah,al		;88c4
mov ch,al		;88c5
mov dh,al		;88c6
mov bh,al		;88c7
mov r8b ,al		;4188c0
mov r9b ,al		;4188c1
mov r10b,al		;4188c2
mov r11b,al		;4188c3
mov r12b,al		;4188c4
mov r13b,al		;4188c5
mov r14b,al		;4188c6
mov r15b,al		;4188c7

mov al,cl		;88c8
mov cl,cl		;88c9
mov dl,cl		;88ca
mov bl,cl		;88cb
mov ah,cl		;88cc
mov ch,cl		;88cd
mov dh,cl		;88ce
mov bh,cl		;88cf
mov r8b ,cl		;4188c8
mov r9b ,cl		;4188c9
mov r10b,cl		;4188ca
mov r11b,cl		;4188cb
mov r12b,cl		;4188cc
mov r13b,cl		;4188cd
mov r14b,cl		;4188ce
mov r15b,cl		;4188cf

mov ax,ax		;6689c0
mov cx,ax		;6689c1
mov dx,ax		;6689c2
mov bx,ax		;6689c3
mov sp,ax		;6689c4
mov bp,ax		;6689c5
mov si,ax		;6689c6
mov di,ax		;6689c7
mov r8w ,ax		;664189c0
mov r9w ,ax		;664189c1
mov r10w,ax		;664189c2
mov r11w,ax		;664189c3
mov r12w,ax		;664189c4
mov r13w,ax		;664189c5
mov r14w,ax		;664189c6
mov r15w,ax		;664189c7

mov ax,cx		;6689c8
mov cx,cx		;6689c9
mov dx,cx		;6689ca
mov bx,cx		;6689cb
mov sp,cx		;6689cc
mov bp,cx		;6689cd
mov si,cx		;6689ce
mov di,cx		;6689cf
mov r8w ,cx		;664189c8
mov r9w ,cx		;664189c9
mov r10w,cx		;664189ca
mov r11w,cx		;664189cb
mov r12w,cx		;664189cc
mov r13w,cx		;664189cd
mov r14w,cx		;664189ce
mov r15w,cx		;664189cf

mov eax,eax		;89c0
mov ecx,eax		;89c1
mov edx,eax		;89c2
mov ebx,eax		;89c3
mov esp,eax		;89c4
mov ebp,eax		;89c5
mov esi,eax		;89c6
mov edi,eax		;89c7
mov r8d ,eax	;4189c0
mov r9d ,eax	;4189c1
mov r10d,eax	;4189c2
mov r11d,eax	;4189c3
mov r12d,eax	;4189c4
mov r13d,eax	;4189c5
mov r14d,eax	;4189c6
mov r15d,eax	;4189c7

mov eax,ecx		;89c8
mov ecx,ecx		;89c9
mov edx,ecx		;89ca
mov ebx,ecx		;89cb
mov esp,ecx		;89cc
mov ebp,ecx		;89cd
mov esi,ecx		;89ce
mov edi,ecx		;89cf
mov r8d ,ecx	;4189c8
mov r9d ,ecx	;4189c9
mov r10d,ecx	;4189ca
mov r11d,ecx	;4189cb
mov r12d,ecx	;4189cc
mov r13d,ecx	;4189cd
mov r14d,ecx	;4189ce
mov r15d,ecx	;4189cf

mov rax,rax		;4889c0
mov rcx,rax		;4889c1
mov rdx,rax		;4889c2
mov rbx,rax		;4889c3
mov rsp,rax		;4889c4
mov rbp,rax		;4889c5
mov rsi,rax		;4889c6
mov rdi,rax		;4889c7
mov r8 ,rax		;4989c0
mov r9 ,rax		;4989c1
mov r10,rax		;4989c2
mov r11,rax		;4989c3
mov r12,rax		;4989c4
mov r13,rax		;4989c5
mov r14,rax		;4989c6
mov r15,rax		;4989c7

mov rax,rcx		;4889c8
mov rcx,rcx
mov rdx,rcx
mov rbx,rcx
mov rsp,rcx
mov rbp,rcx
mov rsi,rcx
mov rdi,rcx
mov r8 ,rcx		;4989c8
mov r9 ,rcx
mov r10,rcx
mov r11,rcx
mov r12,rcx
mov r13,rcx
mov r14,rcx
mov r15,rcx

mov rax,rdx		;4889d0
mov rcx,rdx
mov rdx,rdx
mov rbx,rdx
mov rsp,rdx
mov rbp,rdx
mov rsi,rdx
mov rdi,rdx
mov r8 ,rdx		;4989d0
mov r9 ,rdx
mov r10,rdx
mov r11,rdx
mov r12,rdx
mov r13,rdx
mov r14,rdx
mov r15,rdx

mov rax,rbx		;4889d8
mov rcx,rbx
mov rdx,rbx
mov rbx,rbx
mov rsp,rbx
mov rbp,rbx
mov rsi,rbx
mov rdi,rbx
mov r8 ,rbx		;4989d8
mov r9 ,rbx
mov r10,rbx
mov r11,rbx
mov r12,rbx
mov r13,rbx
mov r14,rbx
mov r15,rbx

mov rax,rsp		;4889e0
mov rcx,rsp
mov rdx,rsp
mov rbx,rsp
mov rsp,rsp
mov rbp,rsp
mov rsi,rsp
mov rdi,rsp
mov r8 ,rsp		;4989e0
mov r9 ,rsp
mov r10,rsp
mov r11,rsp
mov r12,rsp
mov r13,rsp
mov r14,rsp
mov r15,rsp

mov rax,rbp		;4889e8
mov rcx,rbp
mov rdx,rbp
mov rbx,rbp
mov rsp,rbp
mov rbp,rbp
mov rsi,rbp
mov rdi,rbp
mov r8 ,rbp		;4989e8
mov r9 ,rbp
mov r10,rbp
mov r11,rbp
mov r12,rbp
mov r13,rbp
mov r14,rbp
mov r15,rbp

mov rax,rsi		;4889f0
mov rcx,rsi
mov rdx,rsi
mov rbx,rsi
mov rsp,rsi
mov rbp,rsi
mov rsi,rsi
mov rdi,rsi
mov r8 ,rsi		;4989f0
mov r9 ,rsi
mov r10,rsi
mov r11,rsi
mov r12,rsi
mov r13,rsi
mov r14,rsi
mov r15,rsi

mov rax,rdi		;4889f8
mov rcx,rdi
mov rdx,rdi
mov rbx,rdi
mov rsp,rdi
mov rbp,rdi
mov rsi,rdi
mov rdi,rdi
mov r8 ,rdi		;4989f8
mov r9 ,rdi
mov r10,rdi
mov r11,rdi
mov r12,rdi
mov r13,rdi
mov r14,rdi
mov r15,rdi

mov rax,r8		;4c89c0
mov rcx,r8
mov rdx,r8
mov rbx,r8
mov rsp,r8
mov rbp,r8
mov rsi,r8
mov rdi,r8
mov r8 ,r8		;4d89c0
mov r9 ,r8
mov r10,r8
mov r11,r8
mov r12,r8
mov r13,r8
mov r14,r8
mov r15,r8

mov rax,r9		;4c89c8
mov rcx,r9
mov rdx,r9
mov rbx,r9
mov rsp,r9
mov rbp,r9
mov rsi,r9
mov rdi,r9
mov r8 ,r9		;4d89c8
mov r9 ,r9
mov r10,r9
mov r11,r9
mov r12,r9
mov r13,r9
mov r14,r9
mov r15,r9

mov rax,r10		;4c89d0
mov rcx,r10
mov rdx,r10
mov rbx,r10
mov rsp,r10
mov rbp,r10
mov rsi,r10
mov rdi,r10
mov r8 ,r10		;4d89d0
mov r9 ,r10
mov r10,r10
mov r11,r10
mov r12,r10
mov r13,r10
mov r14,r10
mov r15,r10

mov rax,r10		;4c89d8
mov rcx,r11
mov rdx,r11
mov rbx,r11
mov rsp,r11
mov rbp,r11
mov rsi,r11
mov rdi,r11
mov r8 ,r11		;4d89d8
mov r9 ,r11
mov r10,r11
mov r11,r11
mov r12,r11
mov r13,r11
mov r14,r11
mov r15,r11

mov rax,r12		;4c89e0
mov rcx,r12
mov rdx,r12
mov rbx,r12
mov rsp,r12
mov rbp,r12
mov rsi,r12
mov rdi,r12
mov r8 ,r12		;4d89e0
mov r9 ,r12
mov r10,r12
mov r11,r12
mov r12,r12
mov r13,r12
mov r14,r12
mov r15,r12

mov rax,r13		;4c89e8
mov rcx,r13
mov rdx,r13
mov rbx,r13
mov rsp,r13
mov rbp,r13
mov rsi,r13
mov rdi,r13
mov r8 ,r13		;4d89e8
mov r9 ,r13
mov r10,r13
mov r11,r13
mov r12,r13
mov r13,r13
mov r14,r13
mov r15,r13

mov rax,r14		;4c89f0
mov rcx,r14
mov rdx,r14
mov rbx,r14
mov rsp,r14
mov rbp,r14
mov rsi,r14
mov rdi,r14
mov r8 ,r14		;4d89f0
mov r9 ,r14
mov r10,r14
mov r11,r14
mov r12,r14
mov r13,r14
mov r14,r14
mov r15,r14

mov rax,r15		;4c89f8
mov rcx,r15
mov rdx,r15
mov rbx,r15
mov rsp,r15
mov rbp,r15
mov rsi,r15
mov rdi,r15
mov r8 ,r15		;4d89f8
mov r9 ,r15
mov r10,r15
mov r11,r15
mov r12,r15
mov r13,r15
mov r14,r15
mov r15,r15
