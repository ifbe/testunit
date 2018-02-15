[bits 64]
xor spl,al
xor bpl,al
xor sil,al
xor dil,al

;[30c0, 30ff]
xor al,al
xor cl,al
xor dl,al
xor bl,al
xor ah,al
xor ch,al
xor dh,al
xor bh,al

xor al,cl
xor cl,cl
xor dl,cl
xor bl,cl
xor ah,cl
xor ch,cl
xor dh,cl
xor bh,cl

xor al,dl
xor cl,dl
xor dl,dl
xor bl,dl
xor ah,dl
xor ch,dl
xor dh,dl
xor bh,dl

xor al,bl
xor cl,bl
xor dl,bl
xor bl,bl
xor ah,bl
xor ch,bl
xor dh,bl
xor bh,bl

;[31c0, 31ff]
xor ax,ax
xor cx,ax
xor dx,ax
xor bx,ax
xor sp,ax
xor bp,ax
xor si,ax
xor di,ax

xor ax,cx
xor cx,cx
xor dx,cx
xor bx,cx
xor sp,cx
xor bp,cx
xor si,cx
xor di,cx

xor eax,eax
xor ecx,eax
xor edx,eax
xor ebx,eax
xor esp,eax
xor ebp,eax
xor esi,eax
xor edi,eax

xor eax,ecx
xor ecx,ecx
xor edx,ecx
xor ebx,ecx
xor esp,ecx
xor ebp,ecx
xor esi,ecx
xor edi,ecx

xor rax,rax
xor rcx,rax
xor rdx,rax
xor rbx,rax
xor rsp,rax
xor rbp,rax
xor rsi,rax
xor rdi,rax

xor rax,rcx
xor rcx,rcx
xor rdx,rcx
xor rbx,rcx
xor rsp,rcx
xor rbp,rcx
xor rsi,rcx
xor rdi,rcx

xor r8d,r8d
xor r9d,r8d
xor r10d,r8d
xor r11d,r8d
xor r12d,r8d
xor r13d,r8d
xor r14d,r8d
xor r15d,r8d

xor r8d,r9d
xor r9d,r9d
xor r10d,r9d
xor r11d,r9d
xor r12d,r9d
xor r13d,r9d
xor r14d,r9d
xor r15d,r9d

xor r8,r8
xor r9,r8
xor r10,r8
xor r11,r8
xor r12,r8
xor r13,r8
xor r14,r8
xor r15,r8

xor r8,r9
xor r9,r9
xor r10,r9
xor r11,r9
xor r12,r9
xor r13,r9
xor r14,r9
xor r15,r9
