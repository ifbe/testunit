#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int bigadd(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max
);
int bigsub(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max
);
int bigmul(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max
);
int bigdiv(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* quotient, int max1,
	u8* remainder, int max2
);
int bigpow(
        u8* base, int bl,
        u8* exp, int el,
        u8* mod, int ml,
        u8* answer, int max,
        u8* t1, int l1,
        u8* t2, int l2
);
void movsb(u8* rdi, u8* rsi, int rcx)
{
        int j;
        if(rdi < rsi)
        {
                for(j=0;j<rcx;j++)
                {
                        rdi[j] = rsi[j];
                }
        }
        else
        {
                for(;rcx>0;rcx--)
                {
                        rdi[rcx-1] = rsi[rcx-1];
                }
        }
}
void printbigint(u8* p, int i)
{
	int j;
	if(i<=0)return;

	printf("0x");
	for(j=i-1;j>=0;j--)printf("%02x",p[j]);
}




void main()
{
	int j;
	int ret;
	u8 a[32];
	u8 b[32];
	u8 c[32];
	u8 d[32];
	u8 e[32];
	u8 f[32];
	for(j=0;j<16;j++)
	{
		a[j] = 0xf0+j;
		b[j] = 0xff-j;
		c[j] = d[j] = e[j] = f[j] = 0;
	}




	//origin
	printf("origin\n");
	printbigint(a,16);
	printf("\n");
	printbigint(b,16);
	printf("\n\n");




	//add
	printbigint(a, 8);
	printf(" + ");
	printbigint(b, 8);
	printf("\n");

	printf("%016llx\n", (*(u64*)a) + (*(u64*)b) );
	ret = bigadd(a, 8, b, 8, c, 9);

	printbigint(c, ret);
	printf("\n\n");




	//sub
	printbigint(a, 8);
	printf(" - ");
	printbigint(b, 8);
	printf("\n");

	printf("%08llx\n", (*(u64*)a) - (*(u64*)b) );
	ret = bigsub(a, 8, b, 8, c, 9);

	printbigint(c, ret);
	printf("\n\n");




	//mul
	printbigint(a, 8);
	printf(" * ");
	printbigint(b, 8);
	printf("\n");

	printf("%016llx\n", (*(u64*)a) * (*(u64*)b) );
	ret = bigmul(a, 8, b, 8, c, 16);

	printbigint(c, ret);
	printf("\n\n");




	//div
	printbigint(a, 8);
	printf(" / ");
	printbigint(b, 4);
	printf("\n");

	printf("%016llx\n",	(*(u64*)a) / (*(u32*)b) );
	ret = bigdiv(a, 8, b, 4, c, 16, d, 16);

	printbigint(c, ret);
	printf("\n\n");




	//pow
	printbigint(a, 16);
	printf(" , ");
	printbigint(b, 16);
	printf(" , ");
	printbigint(c, 16);
	printf("\n");

	ret = bigpow(
		a, 16,
		b, 16,
		c, 16,
		d, 16,
		e, 16,
		f, 16
	);

	printbigint(d, ret);
	printf("\n");
}
