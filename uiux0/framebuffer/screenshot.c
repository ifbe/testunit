#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
void main()
{
	int fdin,fdout;
	int ret,i,j;
	int fbaddr,fbtotalbyte,fboneline;
	char p[32];
	char *fmap;
	unsigned char rb[1920*1080*3];
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	int screensize;
	unsigned char buf[1000];

	fdin=open("/dev/fb0",O_RDWR);
	if(fdin<0)
	{
		printf("fail@open fb0\n");
		goto byebye1;
	}

	fdout=open("/tmp/a.ppm",O_CREAT|O_RDWR);
	if(fdout<0)
	{
		printf("fail@open a.ppm\n");
		goto byebye2;
	}

	ioctl(fdin,FBIOGET_FSCREENINFO,&finfo);
	ioctl(fdin,FBIOGET_VSCREENINFO,&vinfo);
	screensize=vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;
	fbaddr=finfo.smem_start;
	fbtotalbyte=finfo.smem_len;
	fboneline=finfo.line_length;
	printf("fbaddr=%x,fbtotalbyte=%x,fboneline=%x\n",fbaddr,fbtotalbyte,fboneline);
	printf("linelen=%x(%d)\n",finfo.line_length,finfo.line_length);
	printf("vinfo.xres = %d\n",vinfo.xres);
	printf("vinfo.yres = %d\n",vinfo.yres);
	printf("vinfo.bits_per_pixel = %d\n",vinfo.bits_per_pixel);

	if(vinfo.bits_per_pixel != 32)
	{
		printf("wrong bpp\n");
		goto byebye3;
	}

	fmap=mmap(0,screensize,PROT_READ,MAP_SHARED,fdin,0);
	if(fmap==MAP_FAILED)
	{
		printf("mmap failed\n");
		goto byebye3;
	}




	sprintf(p,"P6\n%d %d\n255\n",vinfo.xres,vinfo.yres);
	write(fdout,p,strlen(p));

	for(i=0;i<screensize;i+=4)
	{
		write(fdout,fmap+i+2,1);
		write(fdout,fmap+i+1,1);
		write(fdout,fmap+i,1);
	}

byebye3:
	close(fdout);
byebye2:
	close(fdin);
byebye1:
	return;
}
