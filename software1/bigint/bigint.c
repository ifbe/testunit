#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int bigadd(u8* abuf, int alen, u8* bbuf, int blen, u8* answer, int max)
{
	int j;
	int temp = 0;
	if(alen >= blen)
	{
		for(j=0;j<blen;j++)
		{
			temp += abuf[j] + bbuf[j];
			answer[j] = temp & 0xff;
			temp >>= 8;
		}
		for(;j<alen;j++)
		{
			if(temp == 0)break;

			temp += abuf[j];
			answer[j] = temp & 0xff;
			temp >>= 8;
		}
		if(temp != 0)
		{
			answer[j] = 1;
			j++;
		}
	}
	else	//alen<blen
	{
		for(j=0;j<alen;j++)
		{
			temp += abuf[j] + bbuf[j];
			answer[j] = temp & 0xff;
			temp >>= 8;
		}
		for(;j<blen;j++)
		{
			temp += bbuf[j];
			answer[j] = temp & 0xff;
			temp >>= 8;
		}
		if(temp != 0)
		{
			answer[j] = 1;
			j++;
		}
	}
	return j;
}




int bigsub(u8* abuf, int alen, u8* bbuf, int blen, u8* answer, int max)
{
	int j;
	int temp = 0;
	if(alen > blen)
	{
		for(j=0;j<blen;j++)
		{
			temp = abuf[j] - bbuf[j] - temp;
			if(temp >= 0)
			{
				answer[j] = temp;
				temp = 0;
			}
			else
			{
				answer[j] = temp + 256;
				temp = 1;
			}
		}
		for(;j<alen;j++)
		{
			temp = abuf[j] - temp;
			if(temp >= 0)
			{
				answer[j] = temp;
				break;
			}
			else
			{
				answer[j] = temp + 256;
				temp = 1;
			}
		}
	}
	else	//alen<blen
	{
		for(j=0;j<alen;j++)
		{
			temp = abuf[j] -bbuf[j] - temp;
			if(temp >= 0)
			{
				answer[j] = temp;
				temp = 0;
			}
			else
			{
				answer[j] = temp + 256;
				temp = 1;
			}
		}
		for(;j<=blen;j++)
		{
			temp = -bbuf[j] - temp;
			if(temp >= 0)
			{
				answer[j] = 0;
				break;
			}
			else
			{
				answer[j] = 0xff;
				temp = 1;
			}
		}
	}
	return j;
}




int bigmul_onebyte(
	u8* abuf, int alen,
	u8* bbuf, int index,
	u8* res, int max)
{
	int j;
	int temp = 0;

	for(j=0;j<index;j++)res[j] = 0;
	res += index;

	for(j=0;j<alen;j++)
	{
		temp += abuf[j] * bbuf[index];
		//printf("%x\n",temp);

		res[j] = temp & 0xff;
		temp >>= 8;
	}
	if(temp != 0)
	{
		res[alen] = temp;
		j++;
	}
	return j+index;
}
int bigmul(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max,
	u8* temp, int rsvd)
{
	int j;
	int ret;
	for(j=0;j<alen+blen;j++)answer[j] = 0;

	for(j=0;j<blen;j++)
	{
		ret = bigmul_onebyte(
			abuf, alen,
			bbuf, j,
			temp, 66666666
		);

		bigadd(answer, 66666666, temp, ret, answer, 6666666);
/*
printf("@%016llx%016llx\n%016llx%016llx\n\n",
	*(u64*)(temp+8), *(u64*)temp,
	*(u64*)(answer+8), *(u64*)answer
);
*/
	}
}




int bigdiv_keeptry(
	u8* abuf, int alen,
	u8* bbuf, int blen)
{
	int j,k;

	k = 0;
	while(1)
	{
//printf("%016llx\n",*(u64*)abuf);
		if(alen == blen)
		{
			//还能不能继续了
			for(j=blen-1;j>=0;j--)
			{
				if(abuf[j] < bbuf[j])goto nomore;
				if(abuf[j] > bbuf[j])break;
			}
			if(j<0)goto nomore;

			//还可以减一次
			bigsub(abuf, blen, bbuf, blen, abuf, blen);
			k++;
		}
		else
		{
			bigsub(abuf, alen, bbuf, blen, abuf, alen);
			k++;

			if(abuf[blen] == 0)
			{
				alen = blen;
				continue;
			}
		}//else

		//error
		if(k>0xff)break;
	}//while

nomore:
	return k;
}
int bigdiv(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* quotient, int max1,
	u8* remainder, int max2)
{
	int j,ret;

	//real alen
	j=alen-1;
	for(;j>0;j--)
	{
		if(abuf[j] == 0)alen--;
		else break;
	}

	//real blen
	j=blen-1;
	for(;j>0;j--)
	{
		if(bbuf[j] == 0)blen--;
		else break;
	}
	if( (blen == 1) && (bbuf[0]) )return 0;

	//两种情况都要挪动
	for(j=0;j<alen;j++)
	{
		quotient[j] = 0;
		remainder[j] = abuf[j];
	}
	remainder[alen] = 0;

	//除数比被除数位数多
	if(blen > alen)	return alen;

	//正常开始减
	for(j=alen-blen;j>=0;j--)
	{
		if(remainder[j+blen] == 0)ret = blen;
		else ret = blen+1;

		quotient[j] = bigdiv_keeptry(
			remainder+j, ret,
			bbuf, blen
		);
	}
}




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
