#define u64 long long


static int table[1024];
static double arc;


double cosine(double x)
{
    double ret=0,item=1.0,temp;
    int n=0,i,sign=1;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        temp=item;
        for(i=1;i<=2*n;i++)temp/=i;
        ret+=sign*temp;
        item*=x*x;
        sign *=-1;
        n++;
      }while (temp>1e-10);
return ret;
}


double sine(double x)
{
    int m = 1,i;
    double temp,ret = 0.0;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        i=0;
        if (m%2 == 0){temp= -1.0;}
        else{temp= 1.0;}
        for(i=1;i<=2*m-1;i++){temp = temp * x/i;}
        ret+= temp;
        m++;
    }while (temp<-.0000005||temp>0.0000005);
return ret;
}


void line(int x1,int y1,int x2,int y2)
{
	int x,y;
	if(x1 == x2){
		if(y1<y2){
		for(y=y1;y<y2;y++)
		{
			draw(x1,y,0xffffffff);
		}
		}
		if(y1>y2){
		for(y=y2;y<y1;y++)
		{
			draw(x1,y,0xffffffff);
		}
		}
	}

	else
	{
		double k=(double)(y1-y2) / (double)(x1-x2);
		if(x1<x2) x=x1;
		if(x1>x2) x=x2;

		for(x= -500;x<500;x++)
		{
			y=y1+ (double)(x-x1)*k;
			if(y<300&&y> -300)
				draw(x,y,0xffffffff);
		}
	}
}


void printworld()
{
	//000000000000000
	int i,j;
	double degree=0;
	for(i=-512;i<512;i++)
		for(j= -320;j<320;j++)
			draw(i,j,0x11111111);

	//中心圆
	for(degree= 0;degree<6.28;degree+=0.01)
		draw((int)(100*cosine(degree)),(int)(100*sine(degree)),0xffffffff);

	//直线切圆最上边
	line(-512,100,512,100);

	//
	int ax=(int)(100* cosine(arc) );
	int ay=(int)(100* sine(arc) );
	line(500,ay,ax,ay);
	line(0,-100,ax,ay);

	//竖线
	int nx=(int)( 200*(double)ax / (100+ (double)ay) );
	line(nx,ay,nx,100);

	//
	if(nx>0&&nx<512)table[nx]=ay;
	if(nx>-512&&nx<0)table[nx+1024]=ay;
	for(i=0;i<512;i++)
	{
		if(table[i]!=0) draw(i,table[i],0xffffffff);
	}
	for(i=512;i<1024;i++)
	{
		if(table[i]!=0) draw(i-1024,table[i],0xffffffff);
	}

	//22222222222222222
	writescreen();
}
void main()
{
	int i,j;
	for(i=0;i<1024;i++) table[i]=0;
	while(1)
	{
		//1
		printworld();

		//2
		unsigned long long type;
		unsigned long long key;
		waitevent(&type,&key);
		if(type==0) break;

		//3
		if(key==0x25){
			if(arc<4.71) arc+=0.01;
		}
		if(key==0x27){
			if(arc> -1.57) arc-=0.01;
		}
	}
}
