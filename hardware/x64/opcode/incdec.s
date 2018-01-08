[bits 64]

inc spl		;40fec4
inc bpl		;40fec5
inc sil		;40fec6
inc dil		;40fec7
;inc sph
;inc bph
;inc sih
;inc dih
dec spl		;40fecc
dec bpl		;40fecd
dec sil		;40fece
dec dil		;40fecf
;dec sph
;dec bph
;dec sih
;dec dih

inc al		;fec0
inc cl		;fec1
inc dl		;fec2
inc bl		;fec3
inc ah		;fec4
inc ch		;fec5
inc dh		;fec6
inc bh		;fec7
dec al		;fec8
dec cl		;fec9
dec dl		;fecb
dec bl		;fecb
dec ah		;fecc
dec ch		;fecd
dec dh		;fece
dec bh		;fecf

inc ax		;66ffc0
inc cx		;66ffc1
inc dx		;66ffc2
inc bx		;66ffc3
inc sp		;66ffc4
inc bp		;66ffc5
inc si		;66ffc6
inc di		;66ffc7
dec ax		;66ffc8
dec cx		;66ffc9
dec dx		;66ffca
dec bx		;66ffcb
dec sp		;66ffcc
dec bp		;66ffcd
dec si		;66ffce
dec di		;66ffcf

inc eax		;ffc0
inc ecx		;ffc1
inc edx		;ffc2
inc ebx		;ffc3
inc esp		;ffc4
inc ebp		;ffc5
inc esi		;ffc6
inc edi		;ffc7
dec eax		;ffc8
dec ecx		;ffc9
dec edx		;ffca
dec ebx		;ffcb
dec esp		;ffcc
dec ebp		;ffcd
dec esi		;ffce
dec edi		;ffcf

inc rax		;48ffc0
inc rcx		;48ffc1
inc rdx		;48ffc2
inc rbx		;48ffc3
inc rsp		;48ffc4
inc rbp		;48ffc5
inc rsi		;48ffc6
inc rdi		;48ffc7
dec rax		;48ffc8
dec rcx		;48ffc9
dec rdx		;48ffca
dec rbx		;48ffcb
dec rsp		;48ffcc
dec rbp		;48ffcd
dec rsi		;48ffce
dec rdi		;48ffcf