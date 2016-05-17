#define u64 long long
#define DWORD unsigned int
#define BYTE unsigned char


//总共 < 0x200*0x200*2 = 0x80000 = 512K空间
static BYTE table512[512][512];
static BYTE table256[256][256];
static BYTE table128[128][128];
static BYTE table64[64][64];
static BYTE table32[32][32];
static BYTE table16[16][16];




void init()
{
	int i,j;
	for(i=0;i<512;i++)
		for(j=0;j<512;j++)
			table512[i][j]=0;
}




void cubie512()
{
	//方块
	int x,y;
	for(y=0;y<512;y++)
		for(x=0;x<512;x++)
			if(table512[y][x]!=0)point(x,y,0xffffffff);
			else point(x,y,0);

	//网格
	for(y=32;y<512;y+=32)			//heng
		for(x=0;x<512;x++)
			point(x,y,0x44444444);
	for(x=32;x<512;x+=32)			//shu
		for(y=0;y<512;y++)
			point(x,y,0x44444444);

}
void cubie256()
{
	//convert
	int x,y,i,j;
	for(y=0;y<256;y++)
	{
		for(x=0;x<256;x++)
		{
			//只要有值，这个就是0
			BYTE val=0;
			if( table512[2*y][2*x] != 0 )val=0xff;
			if( table512[2*y][2*x+1] != 0 )val=0xff;
			if( table512[2*y+1][2*x] != 0 )val=0xff;
			if( table512[2*y+1][2*x+1] != 0 )val=0xff;

			//得到数值
			table256[y][x]=val;
		}
	}

	//print
	for(y=0;y<256;y++)
	{
		for(x=0;x<256;x++)
		{
			point(512+x,0+y,table256[y][x]);
		}
	}
}
void cubie128()
{
	//convert
	int x,y,i,j;
	for(y=0;y<128;y++)
	{
		for(x=0;x<128;x++)
		{
			//只要有值，这个就是0
			BYTE val=0;
			if( table256[2*y][2*x] != 0 )val=0xff;
			if( table256[2*y][2*x+1] != 0 )val=0xff;
			if( table256[2*y+1][2*x] != 0 )val=0xff;
			if( table256[2*y+1][2*x+1] != 0 )val=0xff;

			//得到数值
			table128[y][x]=val;
		}
	}

	//print
	for(y=0;y<128;y++)
	{
		for(x=0;x<128;x++)
		{
			point(512+x,256+y,table128[y][x]);
		}
	}
}
void cubie64()
{
	//convert
	int x,y,i,j;
	for(y=0;y<64;y++)
	{
		for(x=0;x<64;x++)
		{
			//只要有值，这个就是0
			BYTE val=0;
			if( table128[2*y][2*x] != 0 )val=0xff;
			if( table128[2*y][2*x+1] != 0 )val=0xff;
			if( table128[2*y+1][2*x] != 0 )val=0xff;
			if( table128[2*y+1][2*x+1] != 0 )val=0xff;

			//得到数值
			table64[y][x]=val;
		}
	}

	//print
	for(y=0;y<64;y++)
	{
		for(x=0;x<64;x++)
		{
			point(512+x,384+y,table64[y][x]);
		}
	}
}
void cubie32()
{
	//convert
	int x,y,i,j;
	for(y=0;y<32;y++)
	{
		for(x=0;x<32;x++)
		{
			//只要有值，这个就是0
			BYTE val=0;
			if( table64[2*y][2*x] != 0 )val=0xff;
			if( table64[2*y][2*x+1] != 0 )val=0xff;
			if( table64[2*y+1][2*x] != 0 )val=0xff;
			if( table64[2*y+1][2*x+1] != 0 )val=0xff;

			//得到数值
			table32[y][x]=val;
		}
	}

	//print
	for(y=0;y<32;y++)
	{
		for(x=0;x<32;x++)
		{
			point(512+x,448+y,table32[y][x]);
		}
	}

	//在每行结尾显示十六进制的数据值
	DWORD theword=0;
	for(y=0;y<32;y++)
	{
		theword=0;
		for(x=0;x<32;x++)
		{
			theword=theword<<1;
			if(table32[y][x] != 0){theword++;}
		}
		hexadecimal32(0x60,y,theword);
	}
}
void cubie16()
{
	//convert
	int x,y,i,j;
	for(y=0;y<16;y++)
	{
		for(x=0;x<16;x++)
		{
			//只要有值，这个就是0
			BYTE val=0;
			if( table32[2*y][2*x] != 0 )val=0xff;
			if( table32[2*y][2*x+1] != 0 )val=0xff;
			if( table32[2*y+1][2*x] != 0 )val=0xff;
			if( table32[2*y+1][2*x+1] != 0 )val=0xff;

			//得到数值
			table16[y][x]=val;
		}
	}

	//print
	for(y=0;y<16;y++)
	{
		for(x=0;x<16;x++)
		{
			point(512+x,480+y,table16[y][x]);
		}
	}

	//在每行结尾显示十六进制的数据值
	DWORD theword=0;
	for(y=0;y<16;y++)
	{
		theword=0;
		for(x=0;x<16;x++)
		{
			theword=theword<<1;
			if(table16[y][x] != 0){theword++;}
		}
		hexadecimal16(0x58,16+y,theword);
	}
}
void printworld()
{
	int x,y;
	int i,j;

	//背景
	for(y=0;y<512;y++)
	{
		for(x=0;x<1024;x++)
		{
			point(x,y,0x44444444);
		}
	}

	//全图512*512
	cubie512();

	//缩略图256*256
	cubie256();

	//缩略图128*128
	cubie128();

	//缩略图64*64
	cubie64();

	//缩略图32*32
	cubie32();

	//缩略图16*16
	cubie16();

	//写屏
	writescreen();
}
void main()
{
	init();

	while(1)
	{
		//1
		printworld();

		//2
		unsigned long long type;
		unsigned long long key;
		waitevent(&type,&key);
		if(type==0)return;
		else if(type==1)
		{
			if(key==0x1b)init();
		}
		else if(type==2)
		{
			int x=key&0xffff;
			int y=(key>>16)&0xffff;

			if(x<512)
			{
				table512[y][x]=~table512[y][x];
			}
		}
		else if(type==3)			//clicked,mousemove
		{
			int x=key&0xffff;
			int y=(key>>16)&0xffff;

			if(x<512)
			{
				table512[y][x]=~table512[y][x];
			}
		}
	}
}
