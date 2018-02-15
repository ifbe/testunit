[bits 64]

;[]
cmp al,al
cmp cl,al
cmp dl,al
cmp bl,al
cmp ah,al
cmp ch,al
cmp dh,al
cmp bh,al

cmp al,cl
cmp cl,cl
cmp dl,cl
cmp bl,cl
cmp ah,cl
cmp ch,cl
cmp dh,cl
cmp bh,cl

;[]
cmp ax,ax
cmp cx,ax
cmp dx,ax
cmp bx,ax
cmp sp,ax
cmp bp,ax
cmp si,ax
cmp di,ax

cmp ax,cx
cmp cx,cx
cmp dx,cx
cmp bx,cx
cmp sp,cx
cmp bp,cx
cmp si,cx
cmp di,cx

;[]
cmp eax,eax
cmp ecx,eax
cmp edx,eax
cmp ebx,eax
cmp esp,eax
cmp ebp,eax
cmp esi,eax
cmp edi,eax

cmp eax,ecx
cmp ecx,ecx
cmp edx,ecx
cmp ebx,ecx
cmp esp,ecx
cmp ebp,ecx
cmp esi,ecx
cmp edi,ecx

;[]
cmp r8d,eax
cmp r9d,eax
cmp r10d,eax
cmp r11d,eax
cmp r12d,eax
cmp r13d,eax
cmp r14d,eax
cmp r15d,eax

cmp r8d,ecx
cmp r9d,ecx
cmp r10d,ecx
cmp r11d,ecx
cmp r12d,ecx
cmp r13d,ecx
cmp r14d,ecx
cmp r15d,ecx

;[]
cmp eax,r8d
cmp ecx,r8d
cmp edx,r8d
cmp ebx,r8d
cmp esp,r8d
cmp ebp,r8d
cmp esi,r8d
cmp edi,r8d

cmp eax,r9d
cmp ecx,r9d
cmp edx,r9d
cmp ebx,r9d
cmp esp,r9d
cmp ebp,r9d
cmp esi,r9d
cmp edi,r9d

;[]
cmp r8d,r8d
cmp r9d,r8d
cmp r10d,r8d
cmp r11d,r8d
cmp r12d,r8d
cmp r13d,r8d
cmp r14d,r8d
cmp r15d,r8d

cmp r8d,r9d
cmp r9d,r9d
cmp r10d,r9d
cmp r11d,r9d
cmp r12d,r9d
cmp r13d,r9d
cmp r14d,r9d
cmp r15d,r9d

;[]
cmp rax,rax
cmp rcx,rax
cmp rdx,rax
cmp rbx,rax
cmp rsp,rax
cmp rbp,rax
cmp rsi,rax
cmp rdi,rax

cmp rax,rcx
cmp rcx,rcx
cmp rdx,rcx
cmp rbx,rcx
cmp rsp,rcx
cmp rbp,rcx
cmp rsi,rcx
cmp rdi,rcx

;[]
cmp r8,rax
cmp r9,rax
cmp r10,rax
cmp r11,rax
cmp r12,rax
cmp r13,rax
cmp r14,rax
cmp r15,rax

cmp r8,rcx
cmp r9,rcx
cmp r10,rcx
cmp r11,rcx
cmp r12,rcx
cmp r13,rcx
cmp r14,rcx
cmp r15,rcx

;[]
cmp rax,r8
cmp rcx,r8
cmp rdx,r8
cmp rbx,r8
cmp rsp,r8
cmp rbp,r8
cmp rsi,r8
cmp rdi,r8

cmp rax,r9
cmp rcx,r9
cmp rdx,r9
cmp rbx,r9
cmp rsp,r9
cmp rbp,r9
cmp rsi,r9
cmp rdi,r9

;[]
cmp r8,r8
cmp r9,r8
cmp r10,r8
cmp r11,r8
cmp r12,r8
cmp r13,r8
cmp r14,r8
cmp r15,r8

cmp r8,r9
cmp r9,r9
cmp r10,r9
cmp r11,r9
cmp r12,r9
cmp r13,r9
cmp r14,r9
cmp r15,r9

cmp al,0
cmp cl,0
cmp dl,0
cmp bl,0
cmp ah,0
cmp ch,0
cmp dh,0
cmp bh,0

cmp ax,0
cmp cx,0
cmp dx,0
cmp bx,0
cmp ax,0
cmp cx,0
cmp dx,0
cmp bx,0

cmp eax,0
cmp ecx,0
cmp edx,0
cmp ebx,0
cmp eax,0
cmp ecx,0
cmp edx,0
cmp ebx,0

cmp rax,0
cmp rcx,0
cmp rdx,0
cmp rbx,0
cmp rax,0
cmp rcx,0
cmp rdx,0
cmp rbx,0
