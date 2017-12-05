#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>




void main()
{
	int x, y, j;
	int fd, ret;
	char delta;
	unsigned char buf[0x100];

       	fd = open("/dev/input/mice", O_RDONLY);
	if(fd <= 0)
	{
		printf("error@open:%d\n",errno);
	}

	x = 0;
	y = 0;
	while(1)
	{
		ret = read(fd, buf, 0x10);
		if(ret < 0)continue;

		for(j=0;j<16;j++)printf("%02x ",buf[j]);
		printf("\n");

		delta = (char)buf[1];
		x += delta;

		delta = (char)buf[2];
		y -= delta;
		printf("%d,%d	",x,y);

		if((buf[0]&1)==1)printf("left	");
		if((buf[0]&2)==2)printf("right	");
		if((buf[0]&4)==4)printf("middle	");
		printf("\n");
	}
}
