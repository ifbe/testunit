[bits 64]
sub spl,al		;4028c4
sub bpl,al		;4028c5
sub sil,al		;4028c6
sub dil,al		;4028c7

sub al,ah		;28e0
sub cl,ah		;28e1
sub dl,ah		;28e2
sub bl,ah		;28e3
sub ah,ah		;28e4
sub ch,ah		;28e5
sub dh,ah		;28e6
sub bh,ah		;28e7

sub al,al		;28c0
sub cl,al		;28c1
sub dl,al		;28c2
sub bl,al		;28c3
sub ah,al		;28c4
sub ch,al		;28c5
sub dh,al		;28c6
sub bh,al		;28c7
sub r8b ,al		;4128c0
sub r9b ,al		;4128c1
sub r10b,al		;4128c2
sub r11b,al		;4128c3
sub r12b,al		;4128c4
sub r13b,al		;4128c5
sub r14b,al		;4128c6
sub r15b,al		;4128c7

sub al,cl		;28c8
sub cl,cl		;28c9
sub dl,cl		;28ca
sub bl,cl		;28cb
sub ah,cl		;28cc
sub ch,cl		;28cd
sub dh,cl		;28ce
sub bh,cl		;28cf
sub r8b ,cl		;4128c8
sub r9b ,cl		;4128c9
sub r10b,cl		;4128ca
sub r11b,cl		;4128cb
sub r12b,cl		;4128cc
sub r13b,cl		;4128cd
sub r14b,cl		;4128ce
sub r15b,cl		;4128cf

sub ax,ax		;6629c0
sub cx,ax		;6629c1
sub dx,ax		;6629c2
sub bx,ax		;6629c3
sub sp,ax		;6629c4
sub bp,ax		;6629c5
sub si,ax		;6629c6
sub di,ax		;6629c7
sub r8w ,ax		;664129c0
sub r9w ,ax		;664129c1
sub r10w,ax		;664129c2
sub r11w,ax		;664129c3
sub r12w,ax		;664129c4
sub r13w,ax		;664129c5
sub r14w,ax		;664129c6
sub r15w,ax		;664129c7

sub ax,cx		;6629c8
sub cx,cx		;6629c9
sub dx,cx		;6629ca
sub bx,cx		;6629cb
sub sp,cx		;6629cc
sub bp,cx		;6629cd
sub si,cx		;6629ce
sub di,cx		;6629cf
sub r8w ,cx		;664129c8
sub r9w ,cx		;664129c9
sub r10w,cx		;664129ca
sub r11w,cx		;664129cb
sub r12w,cx		;664129cc
sub r13w,cx		;664129cd
sub r14w,cx		;664129ce
sub r15w,cx		;664129cf

sub eax,eax		;29c0
sub ecx,eax		;29c1
sub edx,eax		;29c2
sub ebx,eax		;29c3
sub esp,eax		;29c4
sub ebp,eax		;29c5
sub esi,eax		;29c6
sub edi,eax		;29c7
sub r8d ,eax	;4129c0
sub r9d ,eax	;4129c1
sub r10d,eax	;4129c2
sub r11d,eax	;4129c3
sub r12d,eax	;4129c4
sub r13d,eax	;4129c5
sub r14d,eax	;4129c6
sub r15d,eax	;4129c7

sub eax,ecx		;29c8
sub ecx,ecx		;29c9
sub edx,ecx		;29ca
sub ebx,ecx		;29cb
sub esp,ecx		;29cc
sub ebp,ecx		;29cd
sub esi,ecx		;29ce
sub edi,ecx		;29cf
sub r8d ,ecx	;4129c8
sub r9d ,ecx	;4129c9
sub r10d,ecx	;4129ca
sub r11d,ecx	;4129cb
sub r12d,ecx	;4129cc
sub r13d,ecx	;4129cd
sub r14d,ecx	;4129ce
sub r15d,ecx	;4129cf

sub rax,rax		;4829c0
sub rcx,rax		;4829c1
sub rdx,rax		;4829c2
sub rbx,rax		;4829c3
sub rsp,rax		;4829c4
sub rbp,rax		;4829c5
sub rsi,rax		;4829c6
sub rdi,rax		;4829c7
sub r8 ,rax		;4929c0
sub r9 ,rax		;4929c1
sub r10,rax		;4929c2
sub r11,rax		;4929c3
sub r12,rax		;4929c4
sub r13,rax		;4929c5
sub r14,rax		;4929c6
sub r15,rax		;4929c7

sub rax,rcx		;4829c8
sub rcx,rcx
sub rdx,rcx
sub rbx,rcx
sub rsp,rcx
sub rbp,rcx
sub rsi,rcx
sub rdi,rcx
sub r8 ,rcx		;4929c8
sub r9 ,rcx
sub r10,rcx
sub r11,rcx
sub r12,rcx
sub r13,rcx
sub r14,rcx
sub r15,rcx

sub rax,rdx		;4829d0
sub rcx,rdx
sub rdx,rdx
sub rbx,rdx
sub rsp,rdx
sub rbp,rdx
sub rsi,rdx
sub rdi,rdx
sub r8 ,rdx		;4929d0
sub r9 ,rdx
sub r10,rdx
sub r11,rdx
sub r12,rdx
sub r13,rdx
sub r14,rdx
sub r15,rdx

sub rax,rbx		;4829d8
sub rcx,rbx
sub rdx,rbx
sub rbx,rbx
sub rsp,rbx
sub rbp,rbx
sub rsi,rbx
sub rdi,rbx
sub r8 ,rbx		;4929d8
sub r9 ,rbx
sub r10,rbx
sub r11,rbx
sub r12,rbx
sub r13,rbx
sub r14,rbx
sub r15,rbx

sub rax,rsp		;4829e0
sub rcx,rsp
sub rdx,rsp
sub rbx,rsp
sub rsp,rsp
sub rbp,rsp
sub rsi,rsp
sub rdi,rsp
sub r8 ,rsp		;4929e0
sub r9 ,rsp
sub r10,rsp
sub r11,rsp
sub r12,rsp
sub r13,rsp
sub r14,rsp
sub r15,rsp

sub rax,rbp		;4829e8
sub rcx,rbp
sub rdx,rbp
sub rbx,rbp
sub rsp,rbp
sub rbp,rbp
sub rsi,rbp
sub rdi,rbp
sub r8 ,rbp		;4929e8
sub r9 ,rbp
sub r10,rbp
sub r11,rbp
sub r12,rbp
sub r13,rbp
sub r14,rbp
sub r15,rbp

sub rax,rsi		;4829f0
sub rcx,rsi
sub rdx,rsi
sub rbx,rsi
sub rsp,rsi
sub rbp,rsi
sub rsi,rsi
sub rdi,rsi
sub r8 ,rsi		;4929f0
sub r9 ,rsi
sub r10,rsi
sub r11,rsi
sub r12,rsi
sub r13,rsi
sub r14,rsi
sub r15,rsi

sub rax,rdi		;4829f8
sub rcx,rdi
sub rdx,rdi
sub rbx,rdi
sub rsp,rdi
sub rbp,rdi
sub rsi,rdi
sub rdi,rdi
sub r8 ,rdi		;4929f8
sub r9 ,rdi
sub r10,rdi
sub r11,rdi
sub r12,rdi
sub r13,rdi
sub r14,rdi
sub r15,rdi

sub rax,r8		;4c29c0
sub rcx,r8
sub rdx,r8
sub rbx,r8
sub rsp,r8
sub rbp,r8
sub rsi,r8
sub rdi,r8
sub r8 ,r8		;4d29c0
sub r9 ,r8
sub r10,r8
sub r11,r8
sub r12,r8
sub r13,r8
sub r14,r8
sub r15,r8

sub rax,r9		;4c29c8
sub rcx,r9
sub rdx,r9
sub rbx,r9
sub rsp,r9
sub rbp,r9
sub rsi,r9
sub rdi,r9
sub r8 ,r9		;4d29c8
sub r9 ,r9
sub r10,r9
sub r11,r9
sub r12,r9
sub r13,r9
sub r14,r9
sub r15,r9

sub rax,r10		;4c29d0
sub rcx,r10
sub rdx,r10
sub rbx,r10
sub rsp,r10
sub rbp,r10
sub rsi,r10
sub rdi,r10
sub r8 ,r10		;4d29d0
sub r9 ,r10
sub r10,r10
sub r11,r10
sub r12,r10
sub r13,r10
sub r14,r10
sub r15,r10

sub rax,r10		;4c29d8
sub rcx,r11
sub rdx,r11
sub rbx,r11
sub rsp,r11
sub rbp,r11
sub rsi,r11
sub rdi,r11
sub r8 ,r11		;4d29d8
sub r9 ,r11
sub r10,r11
sub r11,r11
sub r12,r11
sub r13,r11
sub r14,r11
sub r15,r11

sub rax,r12		;4c29e0
sub rcx,r12
sub rdx,r12
sub rbx,r12
sub rsp,r12
sub rbp,r12
sub rsi,r12
sub rdi,r12
sub r8 ,r12		;4d29e0
sub r9 ,r12
sub r10,r12
sub r11,r12
sub r12,r12
sub r13,r12
sub r14,r12
sub r15,r12

sub rax,r13		;4c29e8
sub rcx,r13
sub rdx,r13
sub rbx,r13
sub rsp,r13
sub rbp,r13
sub rsi,r13
sub rdi,r13
sub r8 ,r13		;4d29e8
sub r9 ,r13
sub r10,r13
sub r11,r13
sub r12,r13
sub r13,r13
sub r14,r13
sub r15,r13

sub rax,r14		;4c29f0
sub rcx,r14
sub rdx,r14
sub rbx,r14
sub rsp,r14
sub rbp,r14
sub rsi,r14
sub rdi,r14
sub r8 ,r14		;4d29f0
sub r9 ,r14
sub r10,r14
sub r11,r14
sub r12,r14
sub r13,r14
sub r14,r14
sub r15,r14

sub rax,r15		;4c29f8
sub rcx,r15
sub rdx,r15
sub rbx,r15
sub rsp,r15
sub rbp,r15
sub rsi,r15
sub rdi,r15
sub r8 ,r15		;4d29f8
sub r9 ,r15
sub r10,r15
sub r11,r15
sub r12,r15
sub r13,r15
sub r14,r15
sub r15,r15
