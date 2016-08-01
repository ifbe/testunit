#include<stdio.h>
#include<string.h>




static char buf[0x1000];
char* getdir(char* p)
{
	int j = 0;
	int k = -1;
	while(1)
	{
		if(j>0x1000)break;
		if(p[j] == 0)break;
		if(p[j] == '/')k=j;

		buf[j]=p[j];
		j++;
	}

	if(k<0)return p;
	if(k==0)return "/";

	buf[k]=0;
	return buf;
}
char* getfile(char* p)
{
	int j=0;
	int k=0;
	while(1)
	{
		if(p[j] == 0)break;
		if(p[j] == '/')k=j+1;

		j++;
	}
	return p+k;
}
int wildcard(char* first,char* second)
{
	int j=0;
	int k=0;
	//printf("%s,%s\n",first,second);

	while(1)
	{
		if(first[j]==second[k])
		{
			if(first[j]==0)
			{
				return 1;
			}
			else
			{
				j++;
				k++;
				continue;
			}
		}

		if( (first[j]=='?') | (second[k]=='?') )
		{
			j++;
			k++;
			continue;
		}

		else if(first[j]=='*')
		{
			while(first[j]=='*')j++;
			if(first[j]==0)
			{
				return 1;
			}

			while(1)
			{
				if(second[k]==0)return 0;

				if(first[j] == second[k])
				{
					if(wildcard(first+j+1 , second+k+1) != 0)
					{
						return 1;
					}
				}

				k++;
			}
		}

		else if(second[k]=='*')
		{
			while(second[k]=='*')k++;
			if(second[k]==0)return 1;

			while(1)
			{
				if(first[j]==0)return 0;

				if(first[j] == second[k])
				{
					if(wildcard(first+j+1,second+k+1) != 0)
					{
						return 1;
					}
				}

				j++;
			}
		}

		else return 0;
	}

	return 1;
}
