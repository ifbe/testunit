#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int bigshl(u8*, int len, int sh);
int bigshr(u8*, int len, int sh);
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
	u8* ans, int al,
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
int hexstr2bigint(u8* p, u8* q)
{
	int j=0,k=0;
	while(p[j] != 0)j++;
	while(1)
	{
		//byte.low4
		j--;
		if(j < 0)break;
		if( (p[j] >= 'a') && (p[j] <= 'f') )
		{
			q[k] = p[j] - 0x57;
			k++;
		}
		else if( (p[j] >= 0x30) && (p[j] <= 0x39) )
		{
			q[k] = p[j] - 0x30;
			k++;
		}
		else return 0;

		//byte.high4
		j--;
		if(j < 0)break;
		if( (p[j] >= 'a') && (p[j] <= 'f') )
		{
			q[k-1] += (p[j]-0x57) << 4;
		}
		else if( (p[j] >= 0x30) && (p[j] <= 0x39) )
		{
			q[k-1] += (p[j]-0x30) << 4;
		}
		else return 0;
	}

	return k;
}
int prime_yes(u8* buf, int len)
{
	int j,k;
	u8 one[1];
	if(len <= 0)return 0;

	//[0,0xff]
	if(len == 1)
	{
		k = buf[0];
		if(k <= 13)
		{
			if(k == 2)return 1;
			else if(k == 3)return 1;
			else if(k == 5)return 1;
			else if(k == 7)return 1;
			else if(k == 11)return 1;
			else if(k == 13)return 1;
			else return 0;
		}
		else if(k <= 251)
		{
			if((k&1) == 0)return 0;
			for(j=3;j<14;j+=2)
			{
				if((k % j) == 0)return 0;
			}
			return 1;
		}
		else return 0;
	}

	//[0x100,xxx...]
	if((buf[0]&1) == 0)return 0;

	k = 0;
	len = bigsub(
		buf,len,
		one,1,
		buf,len
	);

	while(1)
	{
		k++;
		len = bigshr(buf,len,1);
		if((buf[0]&1) == 1)break;
	}
	printf("k=%d,odd=",k);
	printbigint(buf,len);
	printf("\n");
	return 0;
}
void main()
{
	int ret;

	u8 base[0x1000];
	int bl;
	u8 exp[0x1000];
	int el;
	u8 mod[0x1000];
	int ml;
	u8 ans[0x1000];
	int al;
	u8 t1[0x1000];
	int l1;
	u8 t2[0x1000];
	int l2;
	u8 t3[0x1000];
	int l3;

	u8 basestr[] = "506a6546546546545646d45646c54656b1561a65461654d6546a5465a666a54654d35135135154978979e879841315351f635131a3515afafafafafaf513b5c13d13e513a51b313c548d97e98f798a79798b79879b84653135135135135135135135135135138461451664648464a545b646c546d546e54f65156164749897946132132156487987789798798753313513515496787984351351354f5487d875c5164654654654657798465651564a546a4654d55661f9511981452642626752675267567567526456435";
	u8 expstr[] = "27952654654651657abcdefedcba982349102983410298a34b701928c370d4712a934e701a298f474a922c73a41081e278e0491c2837407102938740128937409182374098127349817230471029384710923874012479123744918237409123740918237401298374091274091287340918274198274091827340198271298475264765765413214567141";
	u8 modstr[] = "98273491654153135159687764351313513535874687687987643315786374912803abcedabdbdbdbdcccccdabbbbbbaeeeeeeffffff2304712958712398571928374102387412098741902837491827340912734891709283749871230947123474";

	bl = hexstr2bigint(basestr, base);
	el = hexstr2bigint(expstr, exp);
	ml = hexstr2bigint(modstr, mod);
printf("%d,%d,%d\n",bl<<3,el<<3,ml<<3);

printf("hex(pow(");
	printbigint(base,bl);
	printf(",");
	printbigint(exp,el);
	printf(",");
	printbigint(mod,ml);
	printf("))\n");

	ret = bigpow( 
		base, bl,
		exp, el,
		mod, ml,
		ans, al,
		t1, l1,
		t2, l2,
		t3, l3
	);

	printbigint(ans, ret);
	printf("\n");
}




void main1(int argc, char** argv)
{
	int ret;
	int len;

	u8 buf[0x1000];
	if(argc <= 1)return;

	len = hexstr2bigint(argv[1], buf);
	if(len == 0)return;
	printbigint(buf,len);
	printf("\n");

	ret = prime_yes(buf, len);
	if(ret > 0)printf("yes\n");
	else printf("no\n");
}
