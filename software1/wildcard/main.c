#include<stdio.h>
int wildcard(char*,char*);




int main(int argc,char** argv)
{
	int ret;
	if(argc<3)return 0;

	ret = wildcard(argv[1],argv[2]);
	if(ret != 0)printf("yes\n");
	else printf("no\n");
}
