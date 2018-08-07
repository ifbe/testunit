#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/epoll.h>
#include <getopt.h>           

#include <fcntl.h>             
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>         
#include <linux/videodev2.h>

struct buffer {
	void *start;
	size_t length;
};

struct buffer *buffers=NULL;
static unsigned int nbuffers=0;
FILE *file_fd[24];
static int read_frame(int fd)
{
	struct v4l2_buffer buf;
	unsigned int i;
	
	memset(&buf,0,sizeof(buf));
	buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory=V4L2_MEMORY_MMAP;

	ioctl(fd,VIDIOC_DQBUF,&buf);
	assert(buf.index<nbuffers);
	printf("buf.index=%d\n",buf.index);
	fseek(file_fd[buf.index],0,SEEK_SET);
	fwrite(buffers[buf.index].start,buffers[buf.index].length,1,file_fd[buf.index]);
	ioctl(fd,VIDIOC_QBUF,&buf);

	return buf.index;
}

int main (int argc,char ** argv)
{
	int fd,i;
	int size;
	
	char buf[20];
	for(i=0;i<24;i++)
	{
		sprintf(buf,"test%d.jpg",i);
		file_fd[i]=fopen(buf,"w");
	}
	fd=open("/dev/video0",O_RDWR);
	struct v4l2_capability cap;
	if(-1==ioctl(fd,VIDIOC_QUERYCAP,&cap))
	{
		printf("VIDIOC_QUERYCAP error\n");
		exit(1);
	}
	printf("Capability Informations:\n");
    	printf(" driver: %s\n", cap.driver);
    	printf(" card: %s\n", cap.card);
    	printf(" bus_info: %s\n", cap.bus_info);
    	printf(" version: %08X\n", cap.version);
    	printf(" capabilities: %08X\n", cap.capabilities);

	struct v4l2_fmtdesc fmtdesc; 
	fmtdesc.index=0; 
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	printf("Support format:\n");

	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}
////////set format/////
	struct v4l2_format fmt;
	memset(&fmt,0,sizeof(fmt));
	
	fmt.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width	= 640;
	fmt.fmt.pix.height	= 480;
	fmt.fmt.pix.pixelformat	= V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field	= V4L2_FIELD_INTERLACED;	
	if(-1==ioctl(fd,VIDIOC_S_FMT,&fmt))
	{
		printf("VIDIOC_S_FMT error\n");
		exit(1);
	}
/////////////
//////////////
	struct v4l2_requestbuffers req;
	memset(&req,0,sizeof(req));

	req.count	= 24;
	req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory	= V4L2_MEMORY_MMAP;
	if(-1==ioctl(fd,VIDIOC_REQBUFS,&req))
	{
		printf("VIDIOC_REQBUFS error\n");
		exit(1);
	}
//////////////////
//////////////////
	buffers=calloc(req.count,sizeof(struct buffer));
	for(nbuffers=0;nbuffers<req.count;++nbuffers)
	{
		struct v4l2_buffer buf;
		memset(&buf,0,sizeof(buf));
		
		buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory	= V4L2_MEMORY_MMAP;
		buf.index	= nbuffers;

		if(-1==ioctl(fd,VIDIOC_QUERYBUF,&buf))
		{
			printf("VIDIOC_QUERYBUF error\n");
			exit(1);
		}		
		buffers[nbuffers].length = buf.length;
		buffers[nbuffers].start = mmap(NULL,
						buf.length,
						PROT_READ|PROT_WRITE,
						MAP_SHARED,
						fd,
						buf.m.offset
						);
		if(MAP_FAILED == buffers[nbuffers].start)
		{
			printf("mmap failed\n");
			exit(1);
		}


		if(-1==ioctl(fd,VIDIOC_QBUF,&buf))
		{
			printf("VIDIOC_QBUF error\n");
			exit(1);
		}

	}	

	int type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1==ioctl(fd,VIDIOC_STREAMON,&type))
	{
		printf("VIDIOC_STREAM error\n");
		exit(1);
	}
	


	while(1)
	{
		int epfd,nfds,ret;
		struct epoll_event ev,events;
		epfd=epoll_create(256);
	
		ev.data.fd=fd;
		ev.events = EPOLLIN | EPOLLET;
		epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);	
		nfds = epoll_wait(epfd, &events, 1, -1);
		if(nfds==-1)
		{	
			printf("epoll error\n");
			exit(1);
		}
		ret=read_frame(fd);
		if(ret==23)
		{
			break;
		}
	}
	
	unmap:
	for (i = 0; i < nbuffers; ++i)
	{
		fclose (file_fd[i]);
		if (-1 == munmap (buffers[i].start, buffers[i].length))
	    		printf ("munmap error");
	}	
	close (fd);
	
	exit (EXIT_SUCCESS);
	return 0;


}



