#include<stdio.h>
#include<time.h>
#include<windows.h>
#define u8 unsigned char
#define u64 unsigned long long
u64 GetTickCount64();




void sleep_us(int t)
{
	Sleep(t/1000);
}
u64 since1970()
{
	return time(0);
}




u64 gettime()
{
	return 1000 * GetTickCount64();
}
u64 getdate()
{
	u64 temp;
	time_t timep;  
	struct tm *p;  
	time(&timep);  
	p = gmtime(&timep);

	//byte7=world

	//byte6=centry, byte5=year
	temp = 1900 + (p->tm_year);
	temp = ((temp/100)<<8) + (temp%100);

	//byte4=month
	temp = (temp<<8) + 1 + (p->tm_mon);

	//byte3=day
	temp = (temp<<8) + (p->tm_mday);

	//byte2=hour
	temp = (temp<<8) + (p->tm_hour);

	//byte1=minute
	temp = (temp<<8) + (p->tm_min);

	//byte0=second
	temp = (temp<<8) + (p->tm_sec);

	return temp;
}




void main()
{
	u64 t;
	u8* p = (u8*)&t;
	while(1)
	{
		t = getdate();
		printf("%d,%d,%d,%d,%d,%d,%d\n",
			p[6],p[5],p[4],p[3],p[2],p[1],p[0]);

		printf("%lld since 1970, %lld since boot\n",
			since1970(), gettime());

		sleep_us(1000*1000);
	}
}
