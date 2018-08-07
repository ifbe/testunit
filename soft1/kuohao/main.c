#include<stdio.h>
#include<stdlib.h>
static char stack[100];
static int rsp=0;
static int max=0;
static int l=0;
static int r=0;




int qianjin()
{
	int j;
	for(;rsp<2*max;rsp++)
	{
		if(l<max)
		{
			stack[rsp] = '(';
			l++;
		}
		else break;
	}
	for(;rsp<2*max;rsp++)stack[rsp] = ')';
	r=max;
}
int houtui()
{
	int x=0,y=0;
	while(1)
	{
		if(rsp==0)break;

		if(stack[rsp-1] == ')')
		{
			y++;
		}

		if(stack[rsp-1] == '(')
		{
			x++;
			l--;

			if(x != y)
			{
				stack[rsp-1] = ')';
				return 1;
			}
		}

		rsp--;
	}
	return 0;
}
void main(int argc, char** argv)
{
	int ret;
	if(argc==1)return;
	max = atoi(argv[1]);

	stack[max*2]=0;
	rsp=0;
	while(1)
	{
		qianjin();
		printf("%s\n",stack);

		ret = houtui();
		if(ret<=0)return;
	}
}
