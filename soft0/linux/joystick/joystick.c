#include <stdio.h>
#include <fcntl.h>
#include <linux/joystick.h>

void main()
{
	int fd;
	int ret;
	struct js_event ev;

	fd = open("/dev/input/js0", O_RDONLY);
	if(fd <= 0){printf("error@open\n");return;}

	while(1)
	{
		ret = read(fd, &ev, sizeof(ev));
		if(ret <= 0)continue;

		printf("time=%-8x,value=%-8x,type=%-8x,number=%-8x\n",
			ev.time, ev.value, ev.type, ev.number);
	}

byebye:
	close(fd);
}
