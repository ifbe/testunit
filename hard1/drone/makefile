all:
	gcc \
	0.main/main.c \
	0.main/library.i2c.c \
	0.main/library.spi.c \
	1.input/input.c \
	1.input/user/user.stdio.c \
	1.input/user/user.bluetooth.c \
	1.input/user/user.socket.c \
	1.input/user/user.futaba.c \
	1.input/sensor/1.mpu9250.c \
	1.input/sensor/1.ak8963.c \
	1.input/sensor/1.bmp280.c \
	1.input/sensor/2.l3gd20.c \
	1.input/sensor/2.lsm303d.c \
	1.input/sensor/2.bmp180.c \
	1.input/sensor/3.mpu6050.c \
	1.input/sensor/3.ms5611.c \
	2.calculate/calculate.c \
	2.calculate/21.filter/kalman.c \
	2.calculate/22.angle/imuupdate.c \
	2.calculate/22.angle/mahony.c \
	2.calculate/23.fusion/fusion.c \
	2.calculate/24.pid/pid.c \
	3.output/pca9685.c \
	-lm -lpthread
	#-lm -lpthread -lwiringPi
clean:
	rm -f *.out *.exe
