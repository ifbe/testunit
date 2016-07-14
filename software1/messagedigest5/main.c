#include<stdio.h>
#include<string.h>


int main(int argc, char* argv[])
{
	int i;

	unsigned char encrypt[1024];     //存放于加密的信息  
	unsigned char decrypt[17];       //存放加密后的结果  
	scanf("%s",encrypt);             //输入加密的字符  
	printf("strlen=%d\n",(int)strlen((char*)encrypt));

	MD5Init();
	MD5Update(encrypt,strlen((char *)encrypt));
	MD5Final(decrypt);

	printf("加密前:%s\n加密后:",encrypt);
	for(i=0; i<16; i++)
	{
		printf("%02x",decrypt[i]);
	}
	printf("\n\n\n加密结束!\n");

	return 0;
}
