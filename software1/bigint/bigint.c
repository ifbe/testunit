#include<stdio.h>
#include<string.h>
#define u8 unsigned char
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
	if(alen >= blen)
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
		if(temp != 0)
		{
			answer[j] = 0xff;
			j++;
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
		printf("%016llx%016llx\n%016llx%016llx\n\n",
			*(u64*)(temp+8), *(u64*)temp,
			*(u64*)(answer+8), *(u64*)answer
		);
*/
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
	ret = bigadd(a, 8, b, 8, c, 9);
	printf("%llx + %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) + (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);

	//sub
	ret = bigsub(a, 8, b, 8, c, 9);
	printf("%llx - %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) - (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);

	//mul1
	//ret = bigmul_onebyte(a, 8, b, 0, c, 100);
	ret = bigmul(a, 8, b, 8, c, 16, d, 16);
	printf("%llx * %llx\n",
		*(u64*)a,
		*(u64*)b
	);
	printf("	%016llx%016llx\n	%016llx%016llx\n",
		(u64)0,
		(*(u64*)a) * (*(u64*)b),
		*(u64*)(c+8),
		*(u64*)c
	);
}
