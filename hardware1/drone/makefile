all:
	gcc \
	0.control/loop.c 0.control/input.c 0.control/libi2c.c \
	1.sensor/sensor.c 1.sensor/mpu6050.c \
	1.sensor/l3gd20.c 1.sensor/lsm303d.c 1.sensor/bmp280.c \
	1.sensor/mpu9250.c 1.sensor/ak8963.c 1.sensor/bmp180.c \
	2.filter/kalman.c \
	3.state/state.c 3.state/imuupdate.c 3.state/mahony.c \
	4.pid/pid.c 4.pid/pca9685.c \
	-lm -lpthread
	#-lm -lpthread -lwiringPi
clean:
	rm -f *.out *.exe
