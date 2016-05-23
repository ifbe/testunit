all:
	gcc \
	control/loop.c control/input.c \
	sensor/sensor.c sensor/mpu6050.c sensor/mpu9250.c sensor/ak8963.c \
	filter/kalman.c \
	state/state.c state/imuupdate.c state/mahony.c \
	motor/pid.c motor/pca9685.c \
	library/i2c.c library/spi.c \
	-lm -lpthread
	#-lm -lpthread -lwiringPi
clean:
	rm -f *.out *.exe
