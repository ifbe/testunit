[bits 64]

;[]
test eax,eax
test ecx,eax
test edx,eax
test ebx,eax
test esp,eax
test ebp,eax
test esi,eax
test edi,eax

test eax,ecx
test ecx,ecx
test edx,ecx
test ebx,ecx
test esp,ecx
test ebp,ecx
test esi,ecx
test edi,ecx

;[]
test r8d,eax
test r9d,eax
test r10d,eax
test r11d,eax
test r12d,eax
test r13d,eax
test r14d,eax
test r15d,eax

test r8d,ecx
test r9d,ecx
test r10d,ecx
test r11d,ecx
test r12d,ecx
test r13d,ecx
test r14d,ecx
test r15d,ecx

;[]
test eax,r8d
test ecx,r8d
test edx,r8d
test ebx,r8d
test esp,r8d
test ebp,r8d
test esi,r8d
test edi,r8d

test eax,r9d
test ecx,r9d
test edx,r9d
test ebx,r9d
test esp,r9d
test ebp,r9d
test esi,r9d
test edi,r9d

;[]
test r8d,r8d
test r9d,r8d
test r10d,r8d
test r11d,r8d
test r12d,r8d
test r13d,r8d
test r14d,r8d
test r15d,r8d

test r8d,r9d
test r9d,r9d
test r10d,r9d
test r11d,r9d
test r12d,r9d
test r13d,r9d
test r14d,r9d
test r15d,r9d

;[]
test al,0
test cl,1
test dl,2
test bl,3
test ah,4
test ch,5
test dh,6
test bh,7

test ax,0
test cx,1
test dx,2
test bx,3
test ax,4
test cx,5
test dx,6
test bx,7

test eax,0
test ecx,1
test edx,2
test ebx,3
test eax,4
test ecx,5
test edx,6
test ebx,7
