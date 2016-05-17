#include<math.h>


int main()
{
    double angle=3.1415*2;
    int x,y; 

    for(x=0;x<1024;x++)
    {
	for(y=0;y<768;y++)
	{
		point(x,y,0x44444444);
	}
    }

    while((angle-=0.001)>0)
    {
        x=(int)(250*cos(angle));
        y=(int)(250*sin(angle));
        draw(x,y,0x000000);
    }
	writescreen();

	int type;
	int key;
	while(1)
	{
		waitevent(&type,&key);
		if(type==0)return 0;
	}
}