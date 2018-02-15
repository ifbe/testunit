<<<<<<< Updated upstream
[bits 64]
shl spl,1		;40d0e4
shl bpl,1		;40d0e5
shl sil,1
shl dil,1
shr spl,1		;40d0ec
shr bpl,1		;40d0ed
shr sil,1
shr dil,1

shl al,1		;d0e0
shl cl,1		;d0e1
shl dl,1
shl bl,1
shl ah,1		;d0e4
shl ch,1		;d0e5
shl dh,1
shl bh,1
shr al,1		;d0e8
shr cl,1		;d0e9
shr dl,1
shr bl,1
shr ah,1		;d0ec
shr ch,1		;d0ed
shr dh,1
shr bh,1

shl r8b,1		;41d0e0
shl r9b,1		;41d0e1
shl r10b,1
shl r11b,1
shl r12b,1
shl r13b,1
shl r14b,1
shl r15b,1
shr r8b,1		;41d0e8
shr r9b,1		;41d0e9
shr r10b,1
shr r11b,1
shr r12b,1
shr r13b,1
shr r14b,1
shr r15b,1

shl ax,1		;66d1e0
shl cx,1		;66d1e1
shl dx,1
shl bx,1
shl sp,1		;66d1e4
shl bp,1		;66d1e5
shl si,1
shl di,1
shr ax,1		;66d1e8
shr cx,1		;66d1e9
shr dx,1
shr bx,1
shr sp,1		;66d1ec
shr bp,1		;66d1ed
shr si,1
shr di,1

shl r8w,1		;6641d1e0
shl r9w,1		;6641d1e1
shl r10w,1
shl r11w,1
shl r12w,1
shl r13w,1
shl r14w,1
shl r15w,1		;6641d1e7
shr r8w,1		;6641d1e8
shr r9w,1		;6641d1e9
shr r10w,1
shr r11w,1
shr r12w,1
shr r13w,1
shr r14w,1
shr r15w,1		;6641d1ef

shl eax,1		;d1e0
shl ecx,1		;d1e1
shl edx,1
shl ebx,1
shl esp,1		;d1e4
shl ebp,1		;d1e5
shl esi,1
shl edi,1
shr eax,1		;d1e8
shr ecx,1		;d1e9
shr edx,1
shr ebx,1
shr esp,1		;d1ec
shr ebp,1		;d1ed
shr esi,1
shr edi,1

shl r8d,1		;41d1e0
shl r9d,1		;41d1e1
shl r10d,1
shl r11d,1
shl r12d,1
shl r13d,1
shl r14d,1
shl r15d,1		;41d1e7
shr r8d,1		;41d1e8
shr r9d,1		;41d1e9
shr r10d,1
shr r11d,1
shr r12d,1
shr r13d,1
shr r14d,1
shr r15d,1		;41d1ef

shl rax,1		;48d1e0
shl rcx,1		;48d1e1
shl rdx,1
shl rbx,1
shl rsp,1		;48d1e4
shl rbp,1		;48d1e5
shl rsi,1
shl rdi,1
shr rax,1		;48d1e8
shr rcx,1		;48d1e9
shr rdx,1
shr rbx,1
shr rsp,1		;48d1ec
shr rbp,1		;48d1ed
shr rsi,1
shr rdi,1

shl r8,1		;49d1e0
shl r9,1		;49d1e1
shl r10,1
shl r11,1
shl r12,1
shl r13,1
shl r14,1
shl r15,1		;49d1e7
shr r8,1		;49d1e8
shr r9,1		;49d1e9
shr r10,1
shr r11,1
shr r12,1
shr r13,1
shr r14,1
shr r15,1		;49d1ef




shl al,2		;c0e002
shl cl,2		;c0e102
shl dl,3
shl bl,4
shl spl,5		;40c0e405
shl bpl,6		;40c0e506
shl sil,7
shl dil,8
shl r8b,2		;41c0e002
shl r9b,2		;41c0e102
shl r10b,2
shl r11b,2
shl r12b,2
shl r13b,2
shl r14b,2
shl r15b,2

shr al,2		;c0e802
shr cl,2		;c0e902
shr dl,3
shr bl,4
shr spl,5		;40c0ec05
shr bpl,6		;40c0ed06
shr sil,7
shr dil,8
shr r8b,2		;41c0e802
shr r9b,2		;41c0e902
shr r10b,2
shr r11b,2
shr r12b,2
shr r13b,2
shr r14b,2
shr r15b,2

shl ax,2		;66c1e002
shl cx,2		;66c1e102
shl dx,3
shl bx,4
shl sp,5		;66c1e405
shl bp,6		;66c1e506
shl si,7
shl di,8
shl r8w,2		;6641c1e002
shl r9w,2		;6641c1e102
shl r10w,2
shl r11w,2
shl r12w,2
shl r13w,2
shl r14w,2
shl r15w,2

shr ax,2		;66c1e802
shr cx,2		;66c1e902
shr dx,3
shr bx,4
shr sp,5		;66c1ec05
shr bp,6		;66c1ed06
shr si,7
shr di,8
shr r8w,2		;6641c1e802
shr r9w,2		;6641c1e902
shr r10w,2
shr r11w,2
shr r12w,2
shr r13w,2
shr r14w,2
shr r15w,2

shl eax,2		;c1e002
shl ecx,2		;c1e102
shl edx,3
shl ebx,4
shl esp,5		;c1e405
shl ebp,6		;c1e506
shl esi,7
shl edi,8
shl r8d,2		;41c1e002
shl r9d,2		;41c1e102
shl r10d,2
shl r11d,2
shl r12d,2
shl r13d,2
shl r14d,2
shl r15d,2

shr eax,2		;c1e802
shr ecx,2		;c1e902
shr edx,3
shr ebx,4
shr esp,5		;c1ec05
shr ebp,6		;c1ed06
shr esi,7
shr edi,8
shr r8d,2		;41c1e802
shr r9d,2		;41c1e902
shr r10d,2
shr r11d,2
shr r12d,2
shr r13d,2
shr r14d,2
shr r15d,2		;41c1ef02

shl rax,2		;48c1e002
shl rcx,2		;48c1e102
shl rdx,3
shl rbx,4
shl rsp,5		;48c1e405
shl rbp,6		;48c1e506
shl rsi,7
shl rdi,8
shr rax,2		;48c1e802
shr rcx,2		;48c1e902
shr rdx,3
shr rbx,4
shr rsp,5		;48c1ec05
shr rbp,6		;48c1ed06
shr rsi,7
shr rdi,8

shl r8,2		;49c1e002
shl r9,2		;49c1e102
shl r10,2
shl r11,2
shl r12,2
shl r13,2
shl r14,2
shl r15,2		;49c1e702
shr r8,2		;49c1e802
shr r9,2		;49c1e902
shr r10,2
shr r11,2
shr r12,2
shr r13,2
shr r14,2
shr r15,2		;49c1ef02




rol spl,1		;40d0c4
rol bpl,1		;40d0c5
rol sil,1
rol dil,1
ror spl,1		;40d0cc
ror bpl,1		;40d0cd
ror sil,1
ror dil,1

rol al,1		;d0c0
rol cl,1		;d0c1
rol dl,1
rol bl,1
rol ah,1		;d0c4
rol ch,1		;d0c5
rol dh,1
rol bh,1
ror al,1		;d0c8
ror cl,1		;d0c9
ror dl,1
ror bl,1
ror ah,1		;d0cc
ror ch,1		;d0cd
ror dh,1
ror bh,1

rol r8b,1		;41d0c0
rol r9b,1		;41d0c1
rol r10b,1
rol r11b,1
rol r12b,1
rol r13b,1
rol r14b,1
rol r15b,1
ror r8b,1		;41d0c8
ror r9b,1		;41d0c9
ror r10b,1
ror r11b,1
ror r12b,1
ror r13b,1
ror r14b,1
ror r15b,1

rol ax,1		;66d1c0
rol cx,1		;66d1c1
rol dx,1
rol bx,1
rol sp,1		;66d1c4
rol bp,1		;66d1c5
rol si,1
rol di,1
ror ax,1		;66d1c8
ror cx,1		;66d1c9
ror dx,1
ror bx,1
ror sp,1		;66d1cc
ror bp,1		;66d1cd
ror si,1
ror di,1

rol r8w,1		;6641d1c0
rol r9w,1		;6641d1c1
rol r10w,1
rol r11w,1
rol r12w,1
rol r13w,1
rol r14w,1
rol r15w,1		;6641d1c7
ror r8w,1		;6641d1c8
ror r9w,1		;6641d1c9
ror r10w,1
ror r11w,1
ror r12w,1
ror r13w,1
ror r14w,1
ror r15w,1		;6641d1cf

rol eax,1		;d1c0
rol ecx,1		;d1c1
rol edx,1
rol ebx,1
rol esp,1		;d1c4
rol ebp,1		;d1c5
rol esi,1
rol edi,1
ror eax,1		;d1c8
ror ecx,1		;d1c9
ror edx,1
ror ebx,1
ror esp,1		;d1cc
ror ebp,1		;d1cd
ror esi,1
ror edi,1

rol r8d,1		;41d1c0
rol r9d,1		;41d1c1
rol r10d,1
rol r11d,1
rol r12d,1
rol r13d,1
rol r14d,1
rol r15d,1		;41d1c7
ror r8d,1		;41d1c8
ror r9d,1		;41d1c9
ror r10d,1
ror r11d,1
ror r12d,1
ror r13d,1
ror r14d,1
ror r15d,1		;41d1cf

rol rax,1		;48d1c0
rol rcx,1		;48d1c1
rol rdx,1
rol rbx,1
rol rsp,1		;48d1c4
rol rbp,1		;48d1c5
rol rsi,1
rol rdi,1
ror rax,1		;48d1c8
ror rcx,1		;48d1c9
ror rdx,1
ror rbx,1
ror rsp,1		;48d1cc
ror rbp,1		;48d1cd
ror rsi,1
ror rdi,1

rol r8,1		;49d1c0
rol r9,1		;49d1c1
rol r10,1
rol r11,1
rol r12,1
rol r13,1
rol r14,1
rol r15,1		;49d1c7
ror r8,1		;49d1c8
ror r9,1		;49d1c9
ror r10,1
ror r11,1
ror r12,1
ror r13,1
ror r14,1
ror r15,1		;49d1cf




rol al,2		;c0c002
rol cl,2		;c0c102
rol dl,3
rol bl,4
rol spl,5		;40c0c405
rol bpl,6		;40c0c506
rol sil,7
rol dil,8
rol r8b,2		;41c0c002
rol r9b,2		;41c0c102
rol r10b,2
rol r11b,2
rol r12b,2
rol r13b,2
rol r14b,2
rol r15b,2

ror al,2		;c0c802
ror cl,2		;c0c902
ror dl,3
ror bl,4
ror spl,5		;40c0cc05
ror bpl,6		;40c0cd06
ror sil,7
ror dil,8
ror r8b,2		;41c0c802
ror r9b,2		;41c0c902
ror r10b,2
ror r11b,2
ror r12b,2
ror r13b,2
ror r14b,2
ror r15b,2

rol ax,2		;66c1c002
rol cx,2		;66c1c102
rol dx,3
rol bx,4
rol sp,5		;66c1c405
rol bp,6		;66c1c506
rol si,7
rol di,8
rol r8w,2		;6641c1c002
rol r9w,2		;6641c1c102
rol r10w,2
rol r11w,2
rol r12w,2
rol r13w,2
rol r14w,2
rol r15w,2

ror ax,2		;66c1c802
ror cx,2		;66c1c902
ror dx,3
ror bx,4
ror sp,5		;66c1cc05
ror bp,6		;66c1cd06
ror si,7
ror di,8
ror r8w,2		;6641c1c802
ror r9w,2		;6641c1c902
ror r10w,2
ror r11w,2
ror r12w,2
ror r13w,2
ror r14w,2
ror r15w,2

rol eax,2		;c1c002
rol ecx,2		;c1c102
rol edx,3
rol ebx,4
rol esp,5		;c1c405
rol ebp,6		;c1c506
rol esi,7
rol edi,8
rol r8d,2		;41c1c002
rol r9d,2		;41c1c102
rol r10d,2
rol r11d,2
rol r12d,2
rol r13d,2
rol r14d,2
rol r15d,2

ror eax,2		;c1c802
ror ecx,2		;c1c902
ror edx,3
ror ebx,4
ror esp,5		;c1cc05
ror ebp,6		;c1cd06
ror esi,7
ror edi,8
ror r8d,2		;41c1c802
ror r9d,2		;41c1c902
ror r10d,2
ror r11d,2
ror r12d,2
ror r13d,2
ror r14d,2
ror r15d,2		;41c1cf02

rol rax,2		;48c1c002
rol rcx,2		;48c1c102
rol rdx,3
rol rbx,4
rol rsp,5		;48c1c405
rol rbp,6		;48c1c506
rol rsi,7
rol rdi,8
ror rax,2		;48c1c802
ror rcx,2		;48c1c902
ror rdx,3
ror rbx,4
ror rsp,5		;48c1cc05
ror rbp,6		;48c1cd06
ror rsi,7
ror rdi,8

rol r8,2		;49c1c002
rol r9,2		;49c1c102
rol r10,2
rol r11,2
rol r12,2
rol r13,2
rol r14,2
rol r15,2		;49c1c702
ror r8,2		;49c1c802
ror r9,2		;49c1c902
ror r10,2
ror r11,2
ror r12,2
ror r13,2
ror r14,2
ror r15,2		;49c1cf02
=======
[bits 64]
shl eax,1
shl ecx,1
shl edx,1
shl ebx,1
shl esp,1
shl ebp,1
shl esi,1
shl edi,1

shr eax,1
shr ecx,1
shr edx,1
shr ebx,1
shr esp,1
shr ebp,1
shr esi,1
shr edi,1

shl eax,4
shl ecx,4
shl edx,4
shl ebx,4
shl esp,4
shl ebp,4
shl esi,4
shl edi,4

shr eax,4
shr ecx,4
shr edx,4
shr ebx,4
shr esp,4
shr ebp,4
shr esi,4
shr edi,4

shl rax,4
shl rcx,4
shl rdx,4
shl rbx,4
shl rsp,4
shl rbp,4
shl rsi,4
shl rdi,4

shr rax,4
shr rcx,4
shr rdx,4
shr rbx,4
shr rsp,4
shr rbp,4
shr rsi,4
shr rdi,4

shl eax,cl
shl ecx,cl
shl edx,cl
shl ebx,cl
shl esp,cl
shl ebp,cl
shl esi,cl
shl edi,cl

shr eax,cl
shr ecx,cl
shr edx,cl
shr ebx,cl
shr esp,cl
shr ebp,cl
shr esi,cl
shr edi,cl

shl r8d,7
shl r9d,7
shl r10d,7
shl r11d,7
shl r12d,7
shl r13d,7
shl r14d,7
shl r15d,7
shr r8d,7
shr r9d,7
shr r10d,7
shr r11d,7
shr r12d,7
shr r13d,7
shr r14d,7
shr r15d,7

shl r8,7
shl r9,7
shl r10,7
shl r11,7
shl r12,7
shl r13,7
shl r14,7
shl r15,7
shr r8,7
shr r9,7
shr r10,7
shr r11,7
shr r12,7
shr r13,7
shr r14,7
shr r15,7
>>>>>>> Stashed changes
