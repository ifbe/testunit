#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

void main()
{
	int fd;
	int ret;
	char* str;
	struct js_event ev;

	fd = open("/dev/input/js0", O_RDONLY);
	if(fd <= 0){printf("error@open\n");return;}

	while(1)
	{
		ret = read(fd, &ev, sizeof(ev));
		if(ret <= 0)continue;

		//printf("time=%-8x,value=%-8x,type=%-8x,number=%-8x\n",ev.time, ev.value, ev.type, ev.number);
		if(ev.type & 2)
		{
			//if(6 == ev.number)printf("gyro.x\n");
			//else if(7 == ev.number)printf("gyro.y\n");
			//else if(8 == ev.number)printf("gyro.z\n");
			//else if(11 == ev.number)printf("accel.x\n");
			//else if(12 == ev.number)printf("accel.y\n");
			//else if(13 == ev.number)printf("accel.z\n");
			if(	(6 == ev.number) |
				(7 == ev.number) |
				(8 == ev.number) |
				(11== ev.number) |
				(12== ev.number) |
				(13== ev.number) )continue;
			else if(0 == ev.number)printf("lx=%d\n", ev.value);
			else if(1 == ev.number)printf("ly=%d\n", -ev.value);
			else if(2 == ev.number)printf("rx=%d\n", ev.value);
			else if(3 == ev.number)printf("lt=%d\n", ev.value+32767);
			else if(4 == ev.number)printf("rt=%d\n", ev.value+32767);
			else if(5 == ev.number)printf("ry=%d\n", -ev.value);
			else if(9 == ev.number)printf("lk=%d\n", ev.value);
			else if(10== ev.number)printf("rk=%d\n", -ev.value);
			else printf("axis%d=%d\n", ev.number, ev.value);
		}
		else if(ev.type & 1)
		{
			if(0 == ev.number)str = "x";
			else if(1 == ev.number)str = "a";
			else if(2 == ev.number)str = "b";
			else if(3 == ev.number)str = "y";
			else if(4 == ev.number)str = "lb";
			else if(5 == ev.number)str = "rb";
			else if(6 == ev.number)str = "lt";
			else if(7 == ev.number)str = "rt";
			else if(8 == ev.number)str = "share";
			else if(9 == ev.number)str = "option";
			else if(10== ev.number)str = "ls";
			else if(11== ev.number)str = "rs";
			else if(12== ev.number)str = "!";
			else str = "?";
			printf("%s=%x\n", str, ev.value);
		}
		else printf("init\n");
	}

byebye:
	close(fd);
}
