all:
	gcc \
	0.main/main.c 0.main/library.i2c.c 0.main/library.spi.c \
	1.control/control.c 1.control/input.stdio.c 1.control/input.bluetooth.c 1.control/input.socket.c 1.control/input.futaba.c \
	2.sensor/sensor.c \
	2.sensor/1.mpu9250.c 2.sensor/1.ak8963.c 2.sensor/1.bmp280.c \
	2.sensor/2.l3gd20.c 2.sensor/2.lsm303d.c 2.sensor/2.bmp180.c \
	2.sensor/3.mpu6050.c 2.sensor/3.ms5611.c \
	3.filter/filter.c 3.filter/kalman.c \
	4.state/state.c 4.state/imuupdate.c 4.state/mahony.c \
	5.motor/pid.c 5.motor/pca9685.c \
	-lm -lpthread
	#-lm -lpthread -lwiringPi
clean:
	rm -f *.out *.exe
