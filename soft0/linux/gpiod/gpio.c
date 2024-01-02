#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <gpiod.h>

void setinput(struct gpiod_line* gpioline)
{
	printf("--------input--------\n");
	int ret = gpiod_line_request_input(gpioline, "gpioin");
	if(ret != 0)return;
	printf("gpiod_line_request_input: ret=%d\n", ret);

	int val = gpiod_line_get_value(gpioline);
	printf("gpiod_line_get_value: val=%d\n", val);

	gpiod_line_release(gpioline);
	printf("gpiod_line_release\n");
}
void setoutput(struct gpiod_line* gpioline, int val)
{
	printf("--------output--------\n");
	int ret = gpiod_line_request_output(gpioline, "gpioout", 0);
	if (ret != 0)return;
	printf("gpiod_line_request_output ok\n");

	if (gpiod_line_is_used(gpioline)){
		printf("gpiod_line_is_used\n");
		//goto errorline;
	}

	ret = gpiod_line_set_value(gpioline, val);
	printf("gpiod_line_set_value: val=%d, ret=%d\n", val, ret);

	gpiod_line_release(gpioline);
	printf("gpiod_line_release\n");
}
int printhelp()
{
	printf("./a.out /dev/gpiochip0 4 i\n");
	printf("or\n");
	printf("./a.out /dev/gpiochip0 4 o 1\n");
	return 0;
}
int main(int argc, char** argv)
{
	if(argc < 4)return printhelp();
	char* path = argv[1];
	int pin = atoi(argv[2]);
	int mode = argv[3][0];

	if( (argc < 5) && (mode != 'i') )return printhelp();


	printf("--------open--------\n");
	struct gpiod_chip *gpiochip;
	gpiochip = gpiod_chip_open(path);
	if (gpiochip == NULL)goto errorchip;
	printf("gpiod_chip_open ok\n");

	struct gpiod_line *gpioline;
	gpioline = gpiod_chip_get_line(gpiochip, pin);
	if (gpioline == NULL)goto errorline;
	printf("gpiod_chip_get_line ok\n");

	int val = 0;
	switch(mode){
	case 'i':
		setinput(gpioline);
		break;
	case 'o':
		val = atoi(argv[4]);
		setoutput(gpioline, val);
		break;
	}

normalreturn:
	printf("--------close--------\n");
errorline:
	gpiod_chip_close(gpiochip);
errorchip:
	return 0;
}
