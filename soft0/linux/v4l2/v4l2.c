#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>	     
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
struct buffer{
        void *start;
        size_t length;
}my[24];




void fileout(int j, unsigned char* buf, int len)
{
	char haha[32];
	snprintf(haha, 32, "%d.jpg", j);
	printf("(%s,%d)%x,%x,%x,%x\n",haha, len, buf[0], buf[1], buf[2], buf[3]);
	j = open(haha, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	write(j, buf, len);
	close(j);
}
int main(int argc, char* argv[])
{
	//v4l2_open
	int j,k;
	int fd = open("/dev/video0",O_RDWR);	//|O_NONBLOCK);
	if(fd <= 0)
	{
		printf("error@open /dev/video0\n");
		return 0;
	}

	//v4l2_capability
	struct v4l2_capability cap;
	if(-1==ioctl(fd,VIDIOC_QUERYCAP,&cap))
	{
		printf("VIDIOC_QUERYCAP error\n");
		exit(1);
	}
	printf("v4l2_capability:\n");
    	printf("	driver: %s\n", cap.driver);
    	printf("	card: %s\n", cap.card);
    	printf("	bus_info: %s\n", cap.bus_info);
    	printf("	version: %08X\n", cap.version);
    	printf("	capabilities: %08X\n", cap.capabilities);

	//v4l2_fmtdesc
	struct v4l2_fmtdesc desc; 
	desc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	desc.index=0; 
	printf("v4l2_fmtdesc:\n");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &desc) != -1)
	{
		printf("	%s\n", desc.description);
		desc.index++;
	}
	if (desc.index == 0)
	{
		perror("VIDIOC_ENUM_FMT");
	}

	//v4l2_format
	struct v4l2_format fmt;
	fmt.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width	= 640;
	fmt.fmt.pix.height	= 480;
	fmt.fmt.pix.pixelformat	= V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field	= V4L2_FIELD_INTERLACED;	
	if(ioctl(fd,VIDIOC_S_FMT,&fmt) == -1)
	{
		printf("VIDIOC_S_FMT error\n");
		exit(1);
	}

	//v4l2_requestbuffers
	struct v4l2_requestbuffers req;
	req.count	= 24;
	req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory	= V4L2_MEMORY_MMAP;	//V4L2_MEMORY_USERPTR;
	if(ioctl(fd,VIDIOC_REQBUFS,&req) == -1)
	{
		printf("VIDIOC_REQBUFS error\n");
		exit(1);
	}

	//prepare
	struct v4l2_buffer buf;
	for(j=0;j<24;j++)
	{
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = j;
		if(ioctl(fd,VIDIOC_QUERYBUF,&buf) == -1)
		{
			printf("VIDIOC_QUERYBUF\n");
			exit(1);
		}

		my[j].length = buf.length;
		my[j].start = mmap(
			NULL,
			buf.length,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			fd,
			buf.m.offset
		);

		if(my[j].start == MAP_FAILED)
		{
			printf("fail@mmap\n");
			exit(1);
		}
		if(ioctl(fd,VIDIOC_QBUF,&buf) == -1)
		{
			printf("VIDEOC_QBUF\n");
			exit(1);
		}
	}

	//start
	j = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl(fd, VIDIOC_STREAMON, &j) == -1)
        {
                printf("error@ON\n");
                exit(1);
        }

	//record
	int epfd;
	struct epoll_event ev;
	epfd=epoll_create(256);
	ev.data.fd=fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
	for(j=0;j<24;j++)
	{
		//!!!!!!!!!!!!!!must take out ontime!!!!!!!!!!!!!!
                k = epoll_wait(epfd, &ev, 1, -1);
                if(k == -1)
                {
                        printf("epoll error\n");
                        exit(1);
                }

		//
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		ioctl(fd, VIDIOC_DQBUF, my[j].start);
		fileout(j, my[j].start, my[j].length);
		//ioctl(fd, VIDIOC_QBUF, &buf);
	}

	//stop
	for(j=0;j<24;j++)
        {
		munmap(my[j].start, my[j].length);
        }

	//v4l2_release
	close(fd);
}
