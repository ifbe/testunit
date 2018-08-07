#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

static unsigned char data[0x100000];
int writeornot(int fd, char* buf, int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		if(buf[j] == '_')return 0;
	}
	return write(fd, buf, len);
}
int doit_line(char* buf, int len)
{
	int fd=0;
	int j,k,c;
	//printf("%.*s\n", len, buf);

	k = 0;
	c = 0;
	for(j=0;j<=len;j++)
	{
		if((j == len) | (buf[j] == '	'))
		{
			if(c == 0)
			{
				//printf("%.*s:", j-k, buf+k);
				buf[j] = 0;
				fd = open(buf+k, O_CREAT|O_APPEND|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
			}
			if(c == 7)
			{
				//printf("%.*s\n", j-k, buf+k);
				buf[j] = '\n';
				writeornot(fd, buf+k, j-k+1);
			}
			k = j+1;
			c++;
		}
		else if(buf[j] == '*')buf[j] = '_';
	}
	if(fd != 0)close(fd);
	else printf("error@line\n");
}
int doit_part(char* buf, int off, int len)
{
	int k = off;
	for(;off<len;off++)
	{
		if((buf[off] == 0xa)|(buf[off] == 0xd))
		{
			buf[off] = 0;
			//printf("%s\n",buf+k);
			doit_line(buf+k, off-k);

			k = off+1;
			if(k >= 0x80000)return k;
		}
	}
	return off;
}
int main(int argc,char** argv)
{
	int fd,ret,last;
	if(argc<=1){printf("error@argc\n");return -1;}

	fd = open(argv[1], O_RDONLY);
	if(fd <= 0){printf("error@open\n");return -2;}

	ret = read(fd, data, 0x100000);
	if(ret <= 0){printf("error@read1\n");return -3;}

	if(ret < 0x100000){doit_part(data, 0, ret);return -4;}

	last = doit_part(data, 0, 0x100000);
	while(1)
	{
		for(ret=0;ret<0x80000;ret++)data[ret] = data[ret+0x80000];

		ret = read(fd, data+0x80000, 0x80000);
		if(ret <= 0){printf("error@read2\n");return -5;}

		if(ret < 0x80000)
		{
			last = doit_part(data, last-0x80000, 0x80000+ret);
			doit_part(data+0x80000, last-0x80000, ret);
			break;
		}
		else
		{
			last = doit_part(data, last-0x80000, 0x100000);
		}
	}
	return 0;
}
