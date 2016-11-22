#include<stdio.h>
#include<string.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int bigadd(
	u8* abuf, int alen,
	u8* bbuf, int blen,
	u8* answer, int max)
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
	int j,k=0;
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
			if(j<0)
			{
				if(abuf[0] < bbuf[0])goto nomore;
			}

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
	if( (blen == 1) && (bbuf[0] == 0) )return 0;

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




int bigshl(u8* buf, int len)
{
	int j;
	int haha=0;
	for(j=0;j<len;j++)
	{
		haha += ((u16)buf[j]<<1);
		buf[j] = haha & 0xff;
		haha >>= 8;
	}

	if((haha&1) == 1)
	{
		buf[j] = 1;
		j++;
	}
	return j;
}
int bigshr(u8* buf, int len)
{
	int j;
	int haha=0;
	for(j=len-1;j>0;j--)
	{
		haha = (haha<<8) | buf[j];
		buf[j] = haha>>1;
		haha = (haha<<8) & 0x1;
	}

	if(buf[j-1] == 0)j--;
	return j;
}




//answer = (base^exp)%mod
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
int bigpow_modular(
	u8* base, int bl,
	u8* exp, int el,
	u8* mod, int ml,
	u8* answer, int max,
	u8* t1, int l1,
	u8* t2, int l2,
	u8* t3, int l3)
{
	int j;

	//base %= mod
	movsb(t1, base, bl);
	bigdiv(
		t1, bl,		//dividend
		mod, ml,	//divisor
		t2, l2,		//quotient
		base, bl	//reminder
	);

	//answ=1
	for(j=1;j<2*bl;j++)answer[j] = 0;
	answer[0] = 1;

	//
	while(1)
	{
		//odd num: answer = answer*
		if((exp[0]&1) == 1)
		{
			//answer = (answer*base)%mod
			movsb(t1, answer, bl);
			movsb(t2, base, bl);
			bigmul(
				t1, l1,
				t2, l2,
				answer, max,
				t3, l3
			);
		}

		//
		el = bigshr(exp, el);
		if( (el <= 1)&&(exp[0] == 0) )break;

		//even num
		//base = base * base
		movsb(t1, base, bl);
		movsb(t2, base, bl);
		bigmul(
			t1, bl,
			t2, bl,
			base, bl*2,
			t3, bl
		);

		//base = base % mod
		movsb(t1, base, bl);
		bigdiv(
			t1, bl,
			mod, ml,
			t2, bl,
			base, bl
		);
	}
}
