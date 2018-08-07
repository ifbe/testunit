#include<stdio.h>
void traverse_write(char*);
char* traverse_read();
char* getdir(char*);
char* getfile(char*);
int wildcard(char*,char*);




int main(int argc,char** argv)
{
	char* p;
	if(argc==1)return 0;

	if(argc==2)
	{
		traverse_write(argv[1]);
		while(1)
		{
			p=traverse_read();
			if(p==0)break;

			printf("%s\n%s\n",getdir(p),getfile(p));
		}
	}

	if(argc==3)
	{
		traverse_write(argv[1]);
		while(1)
		{
			p=traverse_read();
			if(p==0)break;

			if(wildcard(getfile(p), argv[2]) > 0)
			{
				printf("%s\n%s\n",getdir(p),getfile(p));
			}
		}
	}
}
