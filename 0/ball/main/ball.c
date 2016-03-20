static int x=-250,y=0,xx=8,yy=5;


void cubie(int x,int y,int z)
{
    int i,j;
    for(i=-5;i<5;i++)
        for(j=-5;j<5;j++)
            point(x+512+i,384-y+j,z);
}

void move()
{
    cubie(x,y,0);
    x+=xx;
    y+=yy;
    if((y>360)|(y<-360))
    {
        yy=-yy;
    }
    if((x>-20)|(x<-480))
    {
        xx=-xx;
    }
	cubie(x,y,0xff00);
	writescreen();
}
void main()
{
    while(1)
    {
		//1
		move();

		//2
		int type;
		int key;
		waitevent(&type,&key);
		if(type==0) break;

		//3
		if(type==3) move();
    }
}

