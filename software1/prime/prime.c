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




int prime_yes(u8* addr, int len)
{
	int j;
	u32 temp;
	u8 quotient[16];
	u8 remainder[16];

	if((addr[0]&0x1) == 0)return 0;
	for(temp=3;temp<0xffffffff;temp+=2)
	{
		bigdiv(
			addr, len,
			(u8*)&temp, 4,
			quotient, 16,
			remainder, 16
		);

printf("%08x	%llx	%llx\n",
	temp,
	*(u64*)quotient,
	*(u64*)remainder
);
/*
		for(j=0;j<8;j++)
		{
			if(remainder[j] != 0)return 0;
		}
*/
	}

	return 1;
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
		if(p[j] == 0)break;
		if( (p[j] >= 0x30) && (p[j] <= 0x39) )
		{
			q[k] = p[j] - 0x30;
			k++;
		}
		else return 0;

		//byte.high4
		j--;
		if(j < 0)break;
		if(p[j] == 0)break;
		if( (p[j] >= 0x30) && (p[j] <= 0x39) )
		{
			q[k-1] += (p[j]-0x30) << 4;
		}
		else return 0;
	}

	return k;
}




void main()
{
/*
	u64 buf = 0x5555555555555557;
	int ret = prime_yes((u8*)&buf, 8);

	if(ret > 0)printf("yes\n");
	else printf("no\n");
*/
	int j, len1, len2;
	u8 buf[128];
	u8 temp[128];
	u8 quotient[128];
	u8 remainder[128];
	u8* str1 = "54646546515164654654654657798465651564654654654655661195119814526426267526752675675675265264526546546564625421645645645645645645";
	u8* str2 = "27952752527952726256757496716512645746571465714675416751467541";

	len1 = hexstr2bigint(str1, buf);
	printf("len1=%x\n",len1);
	for(j=0;j<64;j++)
	{
		printf("%02x",buf[63-j]);
	}
	printf("\n");

	len2 = hexstr2bigint(str2, temp);
	printf("len2=%x\n",len2);
	for(j=0;j<64;j++)
	{
		printf("%02x",temp[63-j]);
	}
	printf("\n");

	bigdiv(
		buf, len1,
		temp, len2,
		quotient, 64,
		remainder, 64
	);

	for(j=0;j<64;j++)
	{
		printf("%02x",quotient[63-j]);
	}
	printf("\n");
}
