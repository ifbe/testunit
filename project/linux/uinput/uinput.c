#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/input.h>
#include<linux/uinput.h>
static int fd;




void inituinput()
{
	int ret;

	//
	fd=open("/dev/uinput",O_WRONLY|O_NONBLOCK);
	if(fd<0)
	{
		printf("open fail\n");
		return;
	}

	//
	ioctl(fd, UI_SET_EVBIT, EV_KEY);	//support key button
	ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);	//support mouse left key
	ioctl(fd, UI_SET_EVBIT, EV_REL);	//uinput use relative coordinates
	ioctl(fd, UI_SET_RELBIT, REL_X);	//uinput use x coordinates
	ioctl(fd, UI_SET_RELBIT, REL_Y);	//uinput use y coordinates

	//
	struct uinput_user_dev uidev={
		.name="uinput"
	};
	uidev.id.bustype=BUS_USB;
	uidev.id.vendor=0x4444;
	uidev.id.product=0x2222;
	uidev.id.version=1;
	ret=write(fd,&uidev,sizeof(uidev));
	ret=ioctl(fd,UI_DEV_CREATE);
}
void killuinput()
{
	//
	ioctl(fd,UI_DEV_DESTROY);
	close(fd);
}
void writeuinput(int dx,int dy)
{
	struct input_event ev;
	int ret;

	memset(&ev,0,sizeof(ev));
	ev.type=EV_REL;
	ev.code=REL_X;
	ev.value=dx;
	ret=write(fd,&ev,sizeof(ev));

	memset(&ev,0,sizeof(ev));
	ev.type=EV_REL;
	ev.code=REL_Y;
	ev.value=dy;
	ret=write(fd,&ev,sizeof(ev));

	memset(&ev,0,sizeof(ev));
	ev.type=EV_SYN;
	ev.code=SYN_REPORT;
	ev.value=0;
	ret=write(fd,&ev,sizeof(ev));

	printf("wrote:(%d,%d)\n",dx,dy);
}
