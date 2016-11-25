#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
void movsb(u8* rdi, u8* rsi, int rcx);
void printbigint(u8* p, int i);




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

		for(j=alen;j>0;j--)if(abuf[j-1] != 0)break;
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
			if(temp < 0)
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

	for(j=alen+blen;j>0;j--)if(answer[j-1] != 0)break;
	return j;
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

	//除数比被除数位数多
	if(blen > alen)	return alen;

	//正常开始减
	for(j=alen-blen;j>=0;j--)
	{
		ret = blen;
		if(remainder[j+blen] != 0)
		{
			//not first
			if(j != alen-blen)ret++;
		}

		quotient[j] = bigdiv_keeptry(
			remainder+j, ret,
			bbuf, blen
		);
	}

	for(j=alen-blen;j>=0;j--)
	{
		if(quotient[j] != 0)return j+1;
	}
}
int bigmod(
	u8* abuf, int alen,
	u8* bbuf, int blen,
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
		remainder[j] = abuf[j];
	}

	//除数比被除数位数多
	if(blen > alen)	return alen;

	//正常开始减
	for(j=alen-blen;j>=0;j--)
	{
		ret = blen;
		if(remainder[j+blen] != 0)
		{
			//not first
			if(j != alen-blen)ret++;
		}

		bigdiv_keeptry(
			remainder+j, ret,
			bbuf, blen
		);
	}

	for(j=blen-1;j>=0;j--)
	{
		if(remainder[j] != 0)return j+1;
	}
}




int bigshl(u8* buf, int len, int sh)
{
	int j;
	int haha=0;
	for(j=0;j<len;j++)
	{
		haha += (u16)buf[j]<<8;
		buf[j] = ((haha<<sh)>>8) & 0xff;
		haha >>= 8;
	}

	if(haha != 0)
	{
		buf[len] = haha;
		len++;
	}
	return len;
}
int bigshr(u8* buf, int len, int sh)
{
	int j;
	u16 haha=0;
//printbigint(buf,len);
//printf("(before)\n");
	for(j=len-1;j>=0;j--)
	{
		haha |= buf[j];
		buf[j] = (haha>>sh)&0xff;
		haha <<= 8;
	}
	if(buf[len-1] == 0)len--;
//printbigint(buf,len);
//printf("(after)\n");
	return len;
}




//ans = (base^exp)%mod
int bigpow(
	u8* base, int bl,
	u8* exp, int el,
	u8* mod, int ml,
	u8* ans, int al,
	u8* t1, int l1,
	u8* t2, int l2,
	u8* t3, int l3)
{
	int j;

//printbigint(base,bl);
//printf(" %% ");
//printbigint(mod,ml);
//printf(" = ");
	//base %= mod
	movsb(t1, base, bl);
	bl = bigmod(
		t1, bl,		//dividend
		mod, ml,	//divisor
		base, bl	//reminder
	);
//printbigint(base,bl);
//printf("\n");

	//answ=1
	for(j=1;j<2*bl;j++)ans[j] = 0;
	ans[0] = 1;
	al = 1;

	//
	while(1)
	{
		//odd num
		if((exp[0]&1) == 1)
		{
//printbigint(ans, al);
//printf(" * ");
//printbigint(base, bl);
//printf(" => ");
			//ans *= base
			movsb(t1, ans, al);
			movsb(t2, base, bl);
			al = bigmul(
				t1, al,
				t2, bl,
				ans, al,
				t3, bl
			);
//printbigint(ans, al);
//printf(" => ");

			//ans %= mod
			movsb(t1, ans, al);
			al = bigmod(
				t1, al,
				mod, ml,
				ans, bl
			);
//printbigint(ans, al);
//printf("\n");
		}

		//
		el = bigshr(exp, el, 1);
		if( (el <= 1)&&(exp[0] == 0) )break;

//printbigint(base, bl);
//printf(" * ");
//printbigint(base, bl);
//printf(" => ");
		//even num
		//base = base * base
		movsb(t1, base, bl);
		movsb(t2, base, bl);
		bl = bigmul(
			t1, bl,
			t2, bl,
			base, bl*2,
			t3, bl
		);
//printbigint(base, bl);
//printf(" => ");

		//base = base % mod
		movsb(t1, base, bl);
		bl = bigmod(
			t1, bl,
			mod, ml,
			base, bl
		);
//printbigint(base, bl);
//printf("\n");
	}

	return al;
}
