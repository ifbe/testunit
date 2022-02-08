#include <stdio.h>


int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out shit.yuv\n");
		return 0;
	}

	unsigned char* src = malloc(0x100000);
	FILE* fp = fopen(argv[1],"rb");
	fread(buf,1,0x100000,fp);
	fclose(fp);

	int x,y;
	int y,u,v;
	unsigned char* dst = malloc(0x400000);
	for(y=0;y<128;y++){
		for(x=0;x<128;x++){
			dst[y*128+x] = src[y*128+x];
		}
	}

	FILE* fo = fopen(argv[2],"wb");
	fwrite(buf, 1, 128*128*4, fo);
	fclose(fo);

	free(src);
	free(dst);
	return 0;
}
