#include<math.h>
void point(int x,int y,int color);
void draw(int x,int y,int color);
void setscreen(int x,int y);
void writescreen();
char waitevent();


void userfunc()
{
    double angle=3.1415*2;
    int x,y; 

    for(x=0;x<400;x++)
    {
	for(y=0;y<400;y++)
	{
		point(x,y,0x99999999);
	}
    }

    while((angle-=0.001)>0)
    {
        x=(int)(100*cos(angle));
        y=(int)(100*sin(angle));
        draw(x,y,0x000000);
    }
	writescreen();

	return;
}