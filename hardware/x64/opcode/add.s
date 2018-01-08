[bits 64]
add spl,al		;4000c4
add bpl,al		;4000c5
add sil,al		;4000c6
add dil,al		;4000c7

add al,ah		;00e0
add cl,ah		;00e1
add dl,ah		;00e2
add bl,ah		;00e3
add ah,ah		;00e4
add ch,ah		;00e5
add dh,ah		;00e6
add bh,ah		;00e7

add al,al		;00c0
add cl,al		;00c1
add dl,al		;00c2
add bl,al		;00c3
add ah,al		;00c4
add ch,al		;00c5
add dh,al		;00c6
add bh,al		;00c7
add r8b ,al		;4100c0
add r9b ,al		;4100c1
add r10b,al		;4100c2
add r11b,al		;4100c3
add r12b,al		;4100c4
add r13b,al		;4100c5
add r14b,al		;4100c6
add r15b,al		;4100c7

add al,cl		;00c8
add cl,cl		;00c9
add dl,cl		;00ca
add bl,cl		;00cb
add ah,cl		;00cc
add ch,cl		;00cd
add dh,cl		;00ce
add bh,cl		;00cf
add r8b ,cl		;4100c8
add r9b ,cl		;4100c9
add r10b,cl		;4100ca
add r11b,cl		;4100cb
add r12b,cl		;4100cc
add r13b,cl		;4100cd
add r14b,cl		;4100ce
add r15b,cl		;4100cf

add ax,ax		;6601c0
add cx,ax		;6601c1
add dx,ax		;6601c2
add bx,ax		;6601c3
add sp,ax		;6601c4
add bp,ax		;6601c5
add si,ax		;6601c6
add di,ax		;6601c7
add r8w ,ax		;664101c0
add r9w ,ax		;664101c1
add r10w,ax		;664101c2
add r11w,ax		;664101c3
add r12w,ax		;664101c4
add r13w,ax		;664101c5
add r14w,ax		;664101c6
add r15w,ax		;664101c7

add ax,cx		;6601c8
add cx,cx		;6601c9
add dx,cx		;6601ca
add bx,cx		;6601cb
add sp,cx		;6601cc
add bp,cx		;6601cd
add si,cx		;6601ce
add di,cx		;6601cf
add r8w ,cx		;664101c8
add r9w ,cx		;664101c9
add r10w,cx		;664101ca
add r11w,cx		;664101cb
add r12w,cx		;664101cc
add r13w,cx		;664101cd
add r14w,cx		;664101ce
add r15w,cx		;664101cf

add eax,eax		;01c0
add ecx,eax		;01c1
add edx,eax		;01c2
add ebx,eax		;01c3
add esp,eax		;01c4
add ebp,eax		;01c5
add esi,eax		;01c6
add edi,eax		;01c7
add r8d ,eax	;4101c0
add r9d ,eax	;4101c1
add r10d,eax	;4101c2
add r11d,eax	;4101c3
add r12d,eax	;4101c4
add r13d,eax	;4101c5
add r14d,eax	;4101c6
add r15d,eax	;4101c7

add eax,ecx		;01c8
add ecx,ecx		;01c9
add edx,ecx		;01ca
add ebx,ecx		;01cb
add esp,ecx		;01cc
add ebp,ecx		;01cd
add esi,ecx		;01ce
add edi,ecx		;01cf
add r8d ,ecx	;4101c8
add r9d ,ecx	;4101c9
add r10d,ecx	;4101ca
add r11d,ecx	;4101cb
add r12d,ecx	;4101cc
add r13d,ecx	;4101cd
add r14d,ecx	;4101ce
add r15d,ecx	;4101cf

add rax,rax		;4801c0
add rcx,rax		;4801c1
add rdx,rax		;4801c2
add rbx,rax		;4801c3
add rsp,rax		;4801c4
add rbp,rax		;4801c5
add rsi,rax		;4801c6
add rdi,rax		;4801c7
add r8 ,rax		;4901c0
add r9 ,rax		;4901c1
add r10,rax		;4901c2
add r11,rax		;4901c3
add r12,rax		;4901c4
add r13,rax		;4901c5
add r14,rax		;4901c6
add r15,rax		;4901c7

add rax,rcx		;4801c8
add rcx,rcx
add rdx,rcx
add rbx,rcx
add rsp,rcx
add rbp,rcx
add rsi,rcx
add rdi,rcx
add r8 ,rcx		;4901c8
add r9 ,rcx
add r10,rcx
add r11,rcx
add r12,rcx
add r13,rcx
add r14,rcx
add r15,rcx

add rax,rdx		;4801d0
add rcx,rdx
add rdx,rdx
add rbx,rdx
add rsp,rdx
add rbp,rdx
add rsi,rdx
add rdi,rdx
add r8 ,rdx		;4901d0
add r9 ,rdx
add r10,rdx
add r11,rdx
add r12,rdx
add r13,rdx
add r14,rdx
add r15,rdx

add rax,rbx		;4801d8
add rcx,rbx
add rdx,rbx
add rbx,rbx
add rsp,rbx
add rbp,rbx
add rsi,rbx
add rdi,rbx
add r8 ,rbx		;4901d8
add r9 ,rbx
add r10,rbx
add r11,rbx
add r12,rbx
add r13,rbx
add r14,rbx
add r15,rbx

add rax,rsp		;4801e0
add rcx,rsp
add rdx,rsp
add rbx,rsp
add rsp,rsp
add rbp,rsp
add rsi,rsp
add rdi,rsp
add r8 ,rsp		;4901e0
add r9 ,rsp
add r10,rsp
add r11,rsp
add r12,rsp
add r13,rsp
add r14,rsp
add r15,rsp

add rax,rbp		;4801e8
add rcx,rbp
add rdx,rbp
add rbx,rbp
add rsp,rbp
add rbp,rbp
add rsi,rbp
add rdi,rbp
add r8 ,rbp		;4901e8
add r9 ,rbp
add r10,rbp
add r11,rbp
add r12,rbp
add r13,rbp
add r14,rbp
add r15,rbp

add rax,rsi		;4801f0
add rcx,rsi
add rdx,rsi
add rbx,rsi
add rsp,rsi
add rbp,rsi
add rsi,rsi
add rdi,rsi
add r8 ,rsi		;4901f0
add r9 ,rsi
add r10,rsi
add r11,rsi
add r12,rsi
add r13,rsi
add r14,rsi
add r15,rsi

add rax,rdi		;4801f8
add rcx,rdi
add rdx,rdi
add rbx,rdi
add rsp,rdi
add rbp,rdi
add rsi,rdi
add rdi,rdi
add r8 ,rdi		;4901f8
add r9 ,rdi
add r10,rdi
add r11,rdi
add r12,rdi
add r13,rdi
add r14,rdi
add r15,rdi

add rax,r8		;4c01c0
add rcx,r8
add rdx,r8
add rbx,r8
add rsp,r8
add rbp,r8
add rsi,r8
add rdi,r8
add r8 ,r8		;4d01c0
add r9 ,r8
add r10,r8
add r11,r8
add r12,r8
add r13,r8
add r14,r8
add r15,r8

add rax,r9		;4c01c8
add rcx,r9
add rdx,r9
add rbx,r9
add rsp,r9
add rbp,r9
add rsi,r9
add rdi,r9
add r8 ,r9		;4d01c8
add r9 ,r9
add r10,r9
add r11,r9
add r12,r9
add r13,r9
add r14,r9
add r15,r9

add rax,r10		;4c01d0
add rcx,r10
add rdx,r10
add rbx,r10
add rsp,r10
add rbp,r10
add rsi,r10
add rdi,r10
add r8 ,r10		;4d01d0
add r9 ,r10
add r10,r10
add r11,r10
add r12,r10
add r13,r10
add r14,r10
add r15,r10

add rax,r10		;4c01d8
add rcx,r11
add rdx,r11
add rbx,r11
add rsp,r11
add rbp,r11
add rsi,r11
add rdi,r11
add r8 ,r11		;4d01d8
add r9 ,r11
add r10,r11
add r11,r11
add r12,r11
add r13,r11
add r14,r11
add r15,r11

add rax,r12		;4c01e0
add rcx,r12
add rdx,r12
add rbx,r12
add rsp,r12
add rbp,r12
add rsi,r12
add rdi,r12
add r8 ,r12		;4d01e0
add r9 ,r12
add r10,r12
add r11,r12
add r12,r12
add r13,r12
add r14,r12
add r15,r12

add rax,r13		;4c01e8
add rcx,r13
add rdx,r13
add rbx,r13
add rsp,r13
add rbp,r13
add rsi,r13
add rdi,r13
add r8 ,r13		;4d01e8
add r9 ,r13
add r10,r13
add r11,r13
add r12,r13
add r13,r13
add r14,r13
add r15,r13

add rax,r14		;4c01f0
add rcx,r14
add rdx,r14
add rbx,r14
add rsp,r14
add rbp,r14
add rsi,r14
add rdi,r14
add r8 ,r14		;4d01f0
add r9 ,r14
add r10,r14
add r11,r14
add r12,r14
add r13,r14
add r14,r14
add r15,r14

add rax,r15		;4c01f8
add rcx,r15
add rdx,r15
add rbx,r15
add rsp,r15
add rbp,r15
add rsi,r15
add rdi,r15
add r8 ,r15		;4d01f8
add r9 ,r15
add r10,r15
add r11,r15
add r12,r15
add r13,r15
add r14,r15
add r15,r15
