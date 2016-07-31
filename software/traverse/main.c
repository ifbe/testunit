#include<stdio.h>
void traverse_write(char*);
char* traverse_read();




int main(int argc,char** argv)
{
	char* p;
	if(argc<2)return 0;

	traverse_write(argv[1]);
	while(1)
	{
		p=traverse_read();
		if(p==0)break;

		printf("%s\n",p);
	}
}
