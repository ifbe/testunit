#include<stdio.h>
#include<string.h>




int wildcard(char* first,char* second)
{
	int j=0;
	int k=0;
	printf("%s,%s\n",first,second);

	while(1)
	{
		if(first[j]==second[k])
		{
			if(first[j]==0)
			{
				printf("@1\n");
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
				printf("@2\n");
				return 1;
			}

			while(1)
			{
				if(second[k]==0)return 0;

				if(first[j] == second[k])
				{
					if(wildcard(first+j+1 , second+k+1) != 0)
					{
						printf("@3\n");
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
						printf("@4\n");
						return 1;
					}
				}

				j++;
			}
		}

		else return 0;
	}

	printf("@5\n");
	return 1;
}
