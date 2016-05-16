all:
	gcc main.c \
	mpu9250.c \
	kalman.c \
	quaternion.c \
	control.c \
	pid.c \
	pwm.c \
	library.c \
	-lm -lpthread -lwiringPi
	#-lm -lpthread
clean:
	rm -f *.out *.exe
