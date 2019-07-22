#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		//Needed for SPI port
#include <unistd.h>		//Needed for SPI port
#include <sys/ioctl.h>		//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#define u8 unsigned char
#define u64 unsigned long long
#define PI 3.1415926535897932384626433832795

//0=250dps, 1=500dps, 2=1000dps, 3=2000dps
#define gyr_cfg 2
#define gyr_max 1000

//0=2g, 1=4g, 2=8g, 3=16g
#define acc_cfg 0
#define acc_max 2*9.8

//SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, idle low, data on rising edge, output on falling edge
//SPI_MODE_1 (0,1) 	CPOL = 0, CPHA = 1, idle low, data on falling edge, output on rising edge
//SPI_MODE_2 (1,0) 	CPOL = 1, CPHA = 0, idle high, data on falling edge, output on rising edge
//SPI_MODE_3 (1,1) 	CPOL = 1, CPHA = 1, idle high, data on rising, edge output on falling edge
u8 spi_mode = SPI_MODE_3;
u8 spi_bitsPerWord = 8;
unsigned int spi_speed = 1000000;
int fd;




int SpiOpenPort(char* devstr)
{
	int ret = -1;

	fd = open(devstr, O_RDWR);
	if (fd < 0){
		perror("Error - Could not open SPI device");
		exit(1);
	}

	ret = ioctl(fd, SPI_IOC_WR_MODE, &spi_mode);
	if(ret < 0)
	{
		perror("Could not set SPIMode (WR)...ioctl fail");
		exit(1);
	}

	ret = ioctl(fd, SPI_IOC_RD_MODE, &spi_mode);
	if(ret < 0)
	{
	  perror("Could not set SPIMode (RD)...ioctl fail");
	  exit(1);
	}

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
	if(ret < 0)
	{
	  perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
	  exit(1);
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
	if(ret < 0)
	{
	  perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
	  exit(1);
	}

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if(ret < 0)
	{
	  perror("Could not set SPI speed (WR)...ioctl fail");
	  exit(1);
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if(ret < 0)
	{
	  perror("Could not set SPI speed (RD)...ioctl fail");
	  exit(1);
	}
	return ret;
}
int SpiClosePort()
{
	int ret = close(fd);
	if(ret < 0){
		perror("Error - Could not close SPI device");
		exit(1);
	}
	return ret;
}




int systemspi_read_byte(int fd, u8 reg)
{
        int ret;
        u8 rx_buffer[2];
        u8 tx_buffer[2];
	tx_buffer[0] = reg;

        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx_buffer,
                .rx_buf = (unsigned long)rx_buffer,
                .len = 2,
                //.delay_usecs = delay,
                .speed_hz = spi_speed,
                .bits_per_word = spi_bitsPerWord,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
                perror("can't send spi message");
		return -1;
        }

        return rx_buffer[1];
}
int systemspi_write_byte(int fd, u8 reg, u8 data)
{
        int ret;
        u8 rx_buffer[2];
        u8 tx_buffer[2];
	tx_buffer[0] = reg;
	tx_buffer[1] = data;

        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx_buffer,
                .rx_buf = (unsigned long)rx_buffer,
                .len = 2,
                //.delay_usecs = delay,
                .speed_hz = spi_speed,
                .bits_per_word = spi_bitsPerWord,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
                perror("can't send spi message");
		return -1;
        }

        return rx_buffer[1];
}
int systemspi_read(int fd, u8 reg, u8* buf, int len)
{
	int j,ret;
	for(j=0;j<len;j++){
		ret = systemspi_read_byte(fd, reg+j);
		if(ret < 0)break;

		buf[j] = ret;
	}
	return j;
}
int systemspi_write(int fd, u8 reg, u8* buf, int len)
{
	int j,ret;
	for(j=0;j<len;j++){
		ret = systemspi_write_byte(fd, reg+j, buf[0]);
		if(ret < 0)break;

		buf[j] = ret;
	}
	return j;
}




int ak8963inspi_read(int fd, u8 reg, u8* buf, int len)
{
	u8 tmp[8];

	//MPUREG_I2C_SLV0_ADDR
	systemspi_write_byte(fd, 0x25, 0xc|0x80);

	//MPUREG_I2C_SLV0_REG
	systemspi_write_byte(fd, 0x26, reg);

	//MPUREG_I2C_SLV0_CTRL
	systemspi_write_byte(fd, 0x27, len|0x80);

	//10ms
	usleep(1000*10);

	//read out
	systemspi_read(fd, 0x49 | 0x80, buf, len);
	return 0;
}
int ak8963inspi_write(int fd, u8 reg, u8* buf, int len)
{
	//MPUREG_I2C_SLV0_ADDR
	systemspi_write_byte(fd, 0x25, 0xc);

	//MPUREG_I2C_SLV0_REG
	systemspi_write_byte(fd, 0x26, reg);

	//MPUREG_I2C_SLV0_D0
	systemspi_write_byte(fd, 0x63, buf[0]);

	//MPUREG_I2C_SLV0_CTRL
	systemspi_write_byte(fd, 0x27, 0x81);

	return 0;
}




int main(int argc, char** argv)
{
	int j;
	float f[9];
	int acc[3];
	int gyr[3];
	int mag[3];
	u8 buf[128];
	SpiOpenPort("/dev/spidev0.0");


//check stage
	//check 9250.whoami
	systemspi_read(fd, 0x75|0x80, buf, 1);
	if(0x71 != buf[0]){
		printf("mpu9250.whoami != 0x71\n");
		return 0;
	}


//init mpu9250
	//reset
	systemspi_write_byte(fd, 0x6b, 0x80);
	usleep(1000);

	//clock
	systemspi_write_byte(fd, 0x6b, 0x01);
	usleep(1000);

	//enable
	systemspi_write_byte(fd, 0x6c, 0x00);
	usleep(1000);

	//CONFIG
	systemspi_write_byte(fd, 0x1a, 0x03);
	usleep(1000);

	//SMPLRT_DIV
	systemspi_write_byte(fd, 0x19, 0x04);
	usleep(1000);

	//GYRO_CONFIG
	j = systemspi_read_byte(fd, 0x1b|0x80);
	systemspi_write_byte(fd, 0x1b, (j&0xe7)|(gyr_cfg<<3));
	usleep(1000);

	//ACCEL_CONFIG
	j = systemspi_read_byte(fd, 0x1c|0x80);
	systemspi_write_byte(fd, 0x1c, (j&0xe7)|(acc_cfg<<3));
	usleep(1000);

	//ACCEL_CONFIG2
	j = systemspi_read_byte(fd, 0x1d|0x80);
	systemspi_write_byte(fd, 0x1d, (j&0xf0)|0x3);
	usleep(1000);

	//INT_PIN_CFG
	systemspi_write_byte(fd, 0x37, 0x22);
	usleep(1000);

	//INT_ENABLE
	systemspi_write_byte(fd, 0x38, 0x01);
	usleep(1000);


//enable ak8963
	//MPUREG_USER_CTRL
	systemspi_write_byte(fd, 0x6a, 0x20);
	usleep(1000);

	//MPUREG_I2C_MST_CTRL
	systemspi_write_byte(fd, 0x24, 0xd);
	usleep(1000*10);

	//check 8963.whoami
	ak8963inspi_read(fd, 0, buf, 1);
	if(0x48 != buf[0]){
		printf("ak8963.whoami != 0x48\n");
		return 0;
	}


//init ak8963
	buf[0] = 1;
	ak8963inspi_write(fd, 0xb, buf, 1);
	usleep(1000);

	buf[0] = 0x16;
	ak8963inspi_write(fd, 0xa, buf, 1);
	usleep(1000);


//debug print
	//mpu9250: read must | 0x80
	systemspi_read(fd, 0|0x80, buf, 128);
/*
	for(j=0;j<128;j++){
		printf("%02x ", buf[j]);
		if((j%16) == 15)printf("\n");
	}
*/


//start loop
	usleep(1000*1000);
	while(1){
		//20byte: [0x3b,0x48] + [0x49,0x4e]
		systemspi_write_byte(fd, 0x25, 0xc | 0x80);
		systemspi_write_byte(fd, 0x26, 3);
		systemspi_write_byte(fd, 0x27, 0x87);
		systemspi_read(fd, 0x3b|0x80, buf, 20);


		acc[0] = (buf[0]<<8) | buf[1];
		if(acc[0] > 32767)acc[0] -= 0x10000;

		acc[1] = (buf[2]<<8) | buf[3];
		if(acc[1] > 32767)acc[1] -= 0x10000;

		acc[2] = (buf[4]<<8) | buf[5];
		if(acc[2] > 32767)acc[2] -= 0x10000;

		j = (buf[6]<<8) | buf[7];
		if(j > 32767)j -= 0x10000;

		gyr[0] = (buf[8]<<8) | buf[9];
		if(gyr[0] > 32767)gyr[0] -= 0x10000;

		gyr[1] = (buf[10]<<8) | buf[11];
		if(gyr[1] > 32767)gyr[1] -= 0x10000;

		gyr[2] = (buf[12]<<8) | buf[13];
		if(gyr[2] > 32767)gyr[2] -= 0x10000;

		mag[1] = *(short*)(buf+14);
		mag[0] = *(short*)(buf+16);
		mag[2] = -(*(short*)(buf+18));
/*
		printf(
			"acc:	%d	%d	%d\n"
			"gyr:	%d	%d	%d\n"
			"mag:	%d	%d	%d\n",
			acc[0], acc[1], acc[2],
			gyr[0], gyr[1], gyr[2],
			mag[0], mag[1], mag[2]
		);
*/
		f[0] = gyr[0] * gyr_max / 32768.0 * PI / 180.0;
		f[1] = gyr[1] * gyr_max / 32768.0 * PI / 180.0;
		f[2] = gyr[2] * gyr_max / 32768.0 * PI / 180.0;
		f[3] = acc[0] * acc_max / 32768.0;
		f[4] = acc[1] * acc_max / 32768.0;
		f[5] = acc[2] * acc_max / 32768.0;
		f[6] = mag[0] * 4192.0 / 32760.0;
		f[7] = mag[1] * 4192.0 / 32760.0;
		f[8] = mag[2] * 4192.0 / 32760.0;
		//printf("%f,%f,%f,%f,%f,%f,%f,%f,%f\n",f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);
		//printf("%f,%f,%f\n",f[6],f[7],f[8]);
		printf("%f,%f,%f\n", f[0],f[1],f[2]);
	}

	SpiClosePort(0);
	return 0;
}
