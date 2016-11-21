#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int bigadd(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max);
int bigsub(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max);
int bigmul(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max,
	u8* temp, int rsvd);
int bigdiv(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* quotient, int max1,
	u8* remainder, int max2);




void main()
{
	int j;
	int ret;
	u8 a[16];
	u8 b[16];
	u8 c[16];
	u8 d[16];
	for(j=0;j<16;j++)
	{
		a[j] = 0xc0+j;
		b[j] = 0xcf-j;
		c[j] = d[j] = 0;
	}

	//origin
	printf("origin\n");
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		*(u64*)(a+8),
		*(u64*)a,
		*(u64*)(b+8),
		*(u64*)b
	);

	//add
	printf("%llx + %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	ret = bigadd(a, 8, b, 8, c, 9);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) + (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);

	//sub
	printf("%llx - %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	ret = bigsub(a, 8, b, 8, c, 9);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) - (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);

	//mul
	printf("%llx * %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	//ret = bigmul_onebyte(a, 8, b, 0, c, 100);
	ret = bigmul(a, 8, b, 8, c, 16, d, 16);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) * (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);

	//div
	printf("%llx / %x\n",
		*(u64*)a,
		*(u32*)b
	);
	ret = bigdiv(a, 8, b, 4, c, 16, d, 16);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) / (*(u32*)b),
		*(u64*)(c+8),
		*(u64*)c
	);
}
