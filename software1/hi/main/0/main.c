#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int learn(int argc,char** argv);
int check(int argc,char** argv);
int hash(int argc,char** argv);
int search(int argc,char** argv);
int change(int argc,char** argv);
int insert(int argc,char** argv);
int delete(int argc,char** argv);




void help()
{
	printf("hi.exe\n{\n");
	printf("#step1: learning\n");
	printf("	hi learn aaa.c /some/dir/bbb.cpp /my/folder/haha*\n\n");
	printf("#step2: thinking\n");
	printf("	hi check aaa.seed\n");
	printf("	hi hash bbb.seed\n\n");
	printf("#step3: serving\n");
	printf("	hi search name\n");
	printf("	hi change oldname newname\n");
	printf("	hi insert /main/f1/f2/ name\n");
	printf("	hi delete name\n\n");
	printf("#step4: creating\n");
	printf("	hi 1\n");
	printf("	hi 2\n");
	printf("	hi 3\n");
	printf("	hi 4\n");
	printf("	hi 5\n");
	printf("	hi 6\n");
	printf("	hi 7\n");
	printf("	hi 8\n");
	printf("}\n");
}
int main(int argc,char *argv[])  
{
	//--------------------------help---------------------------
	if(argc==1)
	{
		help();
		return 0;
	}
	//------------------------------------------------------------




	//------------------------------------------------------
	//
	if(strcmp(argv[1] , "learn") == 0)
	{
		learn(argc-1 , argv+1);
	}

	//
	else if(strcmp(argv[1] , "check") == 0)
	{
		printf("@check\n");
	}
	else if(strcmp(argv[1] , "hash") == 0)
	{
		hash(argc-1 , argv+1);
	}

	//
	else if(strcmp(argv[1] , "search") == 0)
	{
		printf("@search\n");
	}
	else if(strcmp(argv[1] , "change") == 0)
	{
		printf("@change\n");
	}
	else if(strcmp(argv[1] , "insert") == 0)
	{
		printf("@insert\n");
	}
	else if(strcmp(argv[1] , "delete") == 0)
	{
		printf("@delete\n");
	}

	//
	else
	{
		printf("bye\n");
	}
	//------------------------------------------------------
}
