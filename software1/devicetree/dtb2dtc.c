#include<stdio.h>
#include<string.h>




//
static unsigned char buf[0x100000*8];	//1m
static int len;

//
static unsigned int magic;		//0
static unsigned int totalsize;		//4
static unsigned int off_dt_struct;	//8
static unsigned int off_dt_strings;	//c
static unsigned int off_dt_mem_rsvmap;	//0x10
static unsigned int version;		//0x14
static unsigned int last_comp_version;	//0x18
static unsigned int boot_cpuid_phys;	//0x1c
static unsigned int size_dt_strings;	//0x20
static unsigned int size_dt_struct;	//0x24

//
static int stack=0;




int explain1(int x)	//start
{
	int j;

	printf("\n");
	for(j=0;j<stack;j++)printf("	");
	printf("%s {\n",buf+x+4);
	stack++;

	j = strlen(buf+x+4) + 5;
	if( (j%4) != 0 )
	{
		j -= j%4;
		j += 4;
	}
	return j;
}
int explain2(int x)	//end
{
	int j;

	stack--;
	for(j=0;j<stack;j++)printf("	");
	printf("};\n");

	return 4;
}
int isstring(int x)
{
	unsigned char haha;

	haha=buf[x];
	if(haha > 0x7a)return 0;
	if(haha < 0x2f)return 0;
	if( (haha > 0x39) && (haha < 0x41) )return 0;
	if( (haha > 0x5a) && (haha < 0x61) )return 0;

	haha=buf[x+1];
	if(haha > 0x7a)return 0;
	if( (haha < 0x2c) && (haha!= 0x20) )return 0;
	if( (haha > 0x39) && (haha < 0x41) )return 0;
	if( (haha > 0x5a) && (haha < 0x5f) )return 0;

	haha=buf[x+2];
	if(haha != 0)
	{
		if(haha > 0x7a)return 0;
		if( (haha < 0x2c) && (haha!= 0x20) )return 0;
		if( (haha > 0x39) && (haha < 0x41) )return 0;
		if( (haha > 0x5a) && (haha < 0x5f) )return 0;
	}

	//
	return 1;
}
int explain3(int x)	//property
{
	int size,offset;
	unsigned int i,j;

	//this
	size=(buf[x+4]<<24) + (buf[x+5]<<16) + (buf[x+6]<<8) + (buf[x+7]);
	offset=(buf[x+8]<<24) + (buf[x+9]<<16) + (buf[x+10]<<8) + (buf[x+11]);

	//print
	for(j=0;j<stack;j++)printf("	");
	printf("%s",buf+off_dt_strings+offset);

	//
	if( size == 0 )
	{
		printf(";\n");
	}
	else if( isstring(x+0xc) )
	{
		i=0;
		printf(" = ");
		while(1)
		{
			printf("\"%s\"",buf+x+0xc+i);
			i += strlen(buf+x+0xc+i) + 1;
			if( i < size )
			{
				printf(", ");
			}
			else
			{
				printf(";\n");
				break;
			}
		}
	}
	else if( (size%4) == 0 )
	{
		printf(" = <");
		for(i=0;i<size;i+=4)
		{
			j = (buf[x+i+0xc]<<24)
			  + (buf[x+i+0xd]<<16)
			  + (buf[x+i+0xe]<<8)
			  + (buf[x+i+0xf]);
			printf("0x%x",j);

			if(i+4<size)printf(" ");
		}
		printf(">;\n");
	}
	else
	{
		printf(" = [");
		for(i=0;i<size;i++)
		{
			printf("%.2x",buf[x+i+0xc]);
			if(i+1<size)printf(" ");
		}
		printf("];\n");
	}

	//next
	size += 0xc;
	if( (size%4) != 0 )
	{
		size -= size%4;
		size += 4;
	}
	return size;
}
int explain4(int x)
{
	printf("??????????????\n");
}
int explain9(int x)
{
}
void main(int argc,char** argv)
{
	int x;
	FILE* src;
	FILE* dst;
	if(argc<3)
	{
		printf("./a.out xxx.dtb xxx.dts\n");
		return;
	}

	//open,read
	src=fopen(argv[1],"rb");
	dst=fopen(argv[2],"wb");

	//
	len=fread(buf,1,0x800000,src);
	off_dt_strings = (buf[12]<<24) + (buf[13]<<16) + (buf[14]<<8) + (buf[15]);

	//in
	stack=1;
	printf("/dts-v1/;\n\n/ {\n");

	//analyse
	x=0x40;
	while(1)	//for(x=0x40;x<len;x+=4)
	{
		if( buf[x+3] == 0x01 )
		{
			//printf("0x1\n");
			x += explain1(x);
		}
		else if( buf[x+3] == 0x02 )
		{
			//printf("0x2\n");
			x += explain2(x);
		}
		else if( buf[x+3] == 0x03 )
		{
			//printf("0x3\n");
			x += explain3(x);
		}
		else if( buf[x+3] == 0x04 )
		{
			//printf("0x4\n");
			x += explain4(x);
		}
		else
		{
			//printf("%x\n",buf[x+3]);
			break;
		}
	}

	//write,close
	//fwrite(buf,1,ret,dst);
	fclose(src);
	fclose(dst);
}





/*
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
*/
