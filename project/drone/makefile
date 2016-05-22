all:
	gcc \
	control/loop.c control/input.c \
	sensor/mpu9250.c sensor/ak8963.c \
	math/kalman.c math/quaternion.c math/pid.c \
	pwm/pca9685.c \
	library/i2c.c \
	-lm -lpthread
	#-lm -lpthread -lwiringPi
clean:
	rm -f *.out *.exe
