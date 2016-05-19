all:
	gcc \
	library.c \
	main.c control.c \
	mpu9250.c kalman.c quaternion.c \
	pid.c pwm.c \
	-lm -lpthread -lwiringPi
	#-lm -lpthread
clean:
	rm -f *.out *.exe
