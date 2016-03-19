#define u64 long long
static int table[16][16];
static int red=0,green=0,blue=0;


void printworld()
{
	//(左边)各种颜色的色板
	int x=0,y=0,color;
	for(x=0;x<256;x++)
	{
		for(y=0;y<256;y++)
		{
			color=red*65536+y*256+x;
			point(2*x,2*y,color);
			point(2*x,2*y+1,color);
			point(2*x+1,2*y,color);
			point(2*x+1,2*y+1,color);
		}
	}

	//(左边)标识位置的小白点
	point(2*blue,2*green,0xffffffff);
	point(2*blue+1,2*green,0xffffffff);
	point(2*blue,2*green+1,0xffffffff);
	point(2*blue+1,2*green+1,0xffffffff);

	//(右边)选中的颜色的方块
	color=(red<<16)+(green<<8)+blue;
	for(x=512;x<1024;x++)
		for(y=0;y<512;y++)
			point(x,y,color);

	//(右边)一串字符告诉用户当前选择的颜色
	for(y=0;y<16;y++)
	{
		for(x=512;x<512+128;x++)
		{
			point(x,y,0xffffffff);
		}
	}
	string(0x40,0,"color= #");
	hexadecimal(0x48,0,color);

	writescreen();
}


void main()
{
	int x,y;
	for(x=0;x<16;x++)
		for(y=0;y<16;y++)
			table[x][y]=0;

	while(1)
	{
		//1
		printworld();

		//2
		unsigned long long type;
		unsigned long long key;
		waitevent(&type,&key);
		if(type==0)return;

		//3
		if(type==1)
		{
			switch(key)
			{
				case '-':	//-
				{
					if(red>0) red--;
					break;
				}
				case '+':	//+
				{
					if(red<255) red++;
					break;
				}
				case 0x28:		//down
				{
					if(green<255) green++;
					break;
				}
				case 0x26:		//up
				{
					if(green>0) green--;
					break;
				}
				case 0x25:		//left
				{
					if(blue>0) blue--;
					break;
				}
				case 0x27:		//right
				{
					if(blue<255) blue++;
					break;
				}
			}
		}
		else if(type==2)
		{
			int x=key&0xffff;
			int y=(key>>16)&0xffff;

			if(x<512)
			{
				if(y<512)
				{
					blue=x/2;
					green=y/2;
				}
			}
		}
	}
}
