#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
float hydropathy[26] = {
	 1.8,  0.0,  2.5, -3.5, -3.5,  2.8, -0.4, -3.2,
	 4.5,  0.0, -3.9,  3.8,  1.9, -3.5,  0.0, -1.6,
	-3.5, -4.5, -0.8, -0.7,  0.0,  4.2, -0.9,  0.0,
	-1.3,  0.0
};
float volume[26] = {
	 88.6,   0.0, 108.5, 111.1, 138.4, 189.9,  60.1, 153.2,
	166.7,   0.0, 168.6, 166.7, 162.9, 114.1,   0.0, 112.7,
	143.8, 173.4,  89.0, 116.1,   0.0, 140.0, 227.8,   0.0,
	193.6,   0.0
};




static unsigned char data[0x100000];
int doit_line(char* buf, int len)
{
	int j;
	float a, b, c;
	//printf("%.*s\n", len, buf);

	a = 0;
	for(j=0;j<len;j++)
	{
		if((buf[j] < 'A')|(buf[j] > 'Z'))
		{
			printf("invalid char\n");
			exit(-1);
		}

		c = hydropathy[buf[j] - 'A'];
		//printf("%f + ", c);
		a += c;
	}
	printf("%f	%f	", a, a/len);

	b = 0;
	for(j=0;j<len;j++)
	{
		if((buf[j] < 'A')|(buf[j] > 'Z'))
		{
			printf("invalid char\n");
			exit(-1);
		}

		c = volume[buf[j] - 'A'];
		//printf("%f + ", c);
		b += c;
	}
	printf("%f	%f	\n", b, b/len);
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
int main(int argc, char** argv)
{
	int j, fd, ret;
	if(argc<=1){printf("error@argc\n");return -1;}

	//fd = open(buf+k, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);

	for(j=1;j<argc;j++)
	{
		printf("%s\n", argv[j]);

		fd = open(argv[j], O_RDONLY);
		if(fd <= 0){printf("error@open\n");return -2;}

		ret = read(fd, data, 0x100000);
		if(ret < 0){printf("error@read1\n");return -3;}

		doit_part(data, 0, 0x100000);
	}
	return 0;
}
