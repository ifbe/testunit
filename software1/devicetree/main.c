#include<stdio.h>
#include<stdlib.h>




static int stack=0;
static unsigned char buf[0x100000*8];	//1m




void main(int argc,char** argv)
{
	int x,y,i,j,len;
	FILE* src;
	FILE* dst;
	unsigned int* haha=(unsigned int*)buf;
	if(argc<3)
	{
		printf("./a.out xxx.dtb xxx.dts\n");
		return;
	}

	//open
	src=fopen(argv[1],"rb");
	dst=fopen(argv[2],"wb");

	//read
	len=fread(buf,1,0x800000,src);
	printf("/ {\n");
	stack=1;

	//analyse
	for(x=0x40;x<len;x+=4)
	{
		if( haha[x/4] == 0x03000000 )
		{
			if( haha[x/4+1] == 0x04000000 )
			{
				if( haha[x/4+2] == 0x00000000 )
				{
					for(i=0;i<stack;i++)printf("	");
printf("#address-cells = <0x%d>;	@%x\n",buf[x+15],x);
				}
				else if( haha[x/4+2] == 0x0f000000 )
				{
					for(i=0;i<stack;i++)printf("	");
printf("#size-cells = <0x%d>;	@%x\n",buf[x+15],x);
				}
			}
			else if( buf[x+7] == 0x28 )
			{
				if( buf[x+0xb] == 0x1b )
				{
					for(i=0;i<stack;i++)printf("	");
printf("model = \"%s\";	@%x\n",buf+x+12,x);
				}
			}
			else if( buf[x+7] == 0x27 )
			{
				if( buf[x+0xb] == 0x21 )
				{
					for(i=0;i<stack;i++)printf("	");
printf("compatible = \"%s\";	@%x\n",buf+x+12,x);
				}
			}
		}
	}

	//write
	printf("};\n");
	//fwrite(buf,1,ret,dst);

	//close
	fclose(src);
	fclose(dst);
}
