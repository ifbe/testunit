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
int bigpow(
        u8* base, int bl,
        u8* exp, int el,
        u8* mod, int ml,
        u8* answer, int max,
        u8* t1, int l1,
        u8* t2, int l2,
        u8* t3, int l3);
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
	u8 g[32];
	for(j=0;j<16;j++)
	{
		a[j] = 0xc0+j;
		b[j] = 0xcf-j;
		c[j] = d[j] = e[j] = f[j] = g[j] = 0;
	}




	//origin
	printf("origin\n");
	printbigint(a,16);
	printf("\n");
	printbigint(b,16);
	printf("\n");




	//add
	printbigint(a, 8);
	printf(" + ");
	printbigint(b, 8);
	printf("\n");

	printf("%016llx\n", (*(u64*)a) + (*(u64*)b) );
	ret = bigadd(a, 8, b, 8, c, 9);

	printbigint(c, ret);
	printf("\n");




	//sub
	printbigint(a, 8);
	printf(" - ");
	printbigint(b, 8);
	printf("\n");

	printf("%08llx\n", (*(u64*)a) - (*(u64*)b) );
	ret = bigsub(a, 8, b, 8, c, 9);

	printbigint(c, ret);
	printf("\n");




	//mul
	//ret = bigmul_onebyte(a, 8, b, 0, c, 100);
	printbigint(a, 8);
	printf(" * ");
	printbigint(b, 8);
	printf("\n");

	printf("%016llx\n", (*(u64*)a) * (*(u64*)b) );
	ret = bigmul(a, 8, b, 8, c, 16, d, 16);

	printbigint(c, ret);
	printf("\n");




	//div
	printbigint(a, 8);
	printf(" / ");
	printbigint(b, 4);
	printf("\n");

	printf("%016llx\n",	(*(u64*)a) / (*(u32*)b) );
	ret = bigdiv(a, 8, b, 4, c, 16, d, 16);

	printbigint(c, ret);
	printf("\n");




	//pow
	printbigint(a, 16);
	printf(" ^ ");
	printbigint(b, 16);
	printf(" %% ");
	printbigint(c, 16);
	printf("\n");

	ret = bigpow(
		a, 16,
		b, 16,
		c, 16,
		d, 16,
		e, 16,
		f, 16,
		g, 16
	);

	printbigint(d, ret);
	printf("\n");
}
