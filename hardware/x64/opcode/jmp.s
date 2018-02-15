00000000  7000              jo 0x2
00000002  7100              jno 0x4
00000004  7200              jc 0x6
00000006  7300              jnc 0x8
00000008  7400              jz 0xa
0000000A  7500              jnz 0xc
0000000C  7600              jna 0xe
0000000E  7700              ja 0x10
00000010  7800              js 0x12
00000012  7900              jns 0x14
00000014  7A00              jpe 0x16
00000016  7B00              jpo 0x18
00000018  7C00              jl 0x1a
0000001A  7D00              jnl 0x1c
0000001C  7E00              jng 0x1e
0000001E  7F00              jg 0x20
00000020  0F8000000000      jo qword 0x26
00000026  0F8100000000      jno qword 0x2c
0000002C  0F8200000000      jc qword 0x32
00000032  0F8300000000      jnc qword 0x38
00000038  0F8400000000      jz qword 0x3e
0000003E  0F8500000000      jnz qword 0x44
00000044  0F8600000000      jna qword 0x4a
0000004A  0F8700000000      ja qword 0x50
00000050  0F8800000000      js qword 0x56
00000056  0F8900000000      jns qword 0x5c
0000005C  0F8A00000000      jpe qword 0x62
00000062  0F8B00000000      jpo qword 0x68
00000068  0F8C00000000      jl qword 0x6e
0000006E  0F8D00000000      jnl qword 0x74
00000074  0F8E00000000      jng qword 0x7a
0000007A  0F8F00000000      jg qword 0x80