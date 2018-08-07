#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int bigdup(u8* src, int sl, u8* dst, int dl);
int bigcmp(u8* src, int sl, u8* dst, int dl);
int bigshl(u8* buf, int len, int sh);
int bigshr(u8* buf, int len, int sh);
int bigadd(
	u8* abuf, int alen,
	u8* bbuf, int blen);
int bigadd_muled(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	int val);
int bigsub(
	u8* abuf, int alen,
	u8* bbuf, int blen);
int bigsub_muled(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	int val);
int bigmul(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max);
int bigpow(
	u8* ans, int al,
	u8* base, int bl,
	u8* exp, int el,
	u8* mod, int ml);
int bigdiv(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* quotient, int max1,
	u8* remainder, int max2);




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
		one,1
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

	u8 ans[0x1000];
	int al;
	u8 base[0x1000];
	int bl;
	u8 exp[0x1000];
	int el;
	u8 mod[0x1000];
	int ml;

	u8 basestr[] = "3051300d0609608648016503040203050004405d439398be8f18b246847428b6999fe9766c7c86f8f2d0996e7849fc457a3b3cc180aea3fcf9843def2abcc556e36cac29b06337353d7f59310aa00b16f0af75";
	u8 expstr[] = "c486ec92cc071f2ce3f6342187582f0b40ba4b5e4808e6c376802ba5f06873192484d77706e691dfbaead77d09369fa5e38b498de5cf1f0ae9ee909415d95ee45548d0be5615cf3c1e4d055d286ea7c631a8254f219aa981dd506dc2411b3d27e8917a1f26380f3e9b12c775355384bd1d094249eea0176e5eb75f5bccb4be5feeb3a81c4addf6dda1804c4a76e6d95866acaadd1cb73501a6b18dc2ffd19d4ccaaac1d05e40ce30b5f691fec3f70e5e4dfc508935f96480480694e50a9a587a79014f5c94b9fb8ec1143cbc8eade3df4e0bba12bd8330d8bb0b1f4cc4f7c717d1039aade3afe07d408beca28ac20c4ed4916135be5b7f9c908ac9396f9d5aa1";
	u8 modstr[] = "c80a4f6a78e4ffe99e998de31db93c813c5286d237bd946d1f76fa9ff6a459ccfe7d32643ca60aaa356674a7eb056b3417315cd842d9511441f9dae804b28edb71f71a0e329b7bc7f9dd950dc197769ba8ae14f97fb57c6143c0a67e9b9c876b7a89d9f1abf9c1698248a4dc93a806df81cf817ad6baa06b17380bd862193861b0aa5c1ae96134bfffc64fe6adba99d4d3b963a6fb1cdd1604ea59eed0cce3796af16966a947f03e6d61e5fc2aea4c1c09983bf08824c1a7649dbf68e8aeafe272d1a62e30303dd70b888bdf9bf999ca01555559b43438d97d7e5b0224f3f585a3f2d0288fc63876903176cfcc8a26e36f1736a53aad17d6d11cc6566dc44b4b";
/*
	u8 basestr[] = "506a6546546546545646d45646c54656b1561a65";
	u8 expstr[] = "27952654654651657abcdefedcba9823";
	u8 modstr[] = "982734916541531351596877643513";
*/
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
		ans, al,
		base, bl,
		exp, el,
		mod, ml
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
