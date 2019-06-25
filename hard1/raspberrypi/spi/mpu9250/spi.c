#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		//Needed for SPI port
#include <unistd.h>		//Needed for SPI port
#include <sys/ioctl.h>		//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#define u8 unsigned char
#define u64 unsigned long long




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

	//MPUREG_USER_CTRL, MPUREG_I2C_MST_CTRL
	systemspi_write_byte(fd, 0x6a, 0x20);
	systemspi_write_byte(fd, 0x24, 0xd);

	//check 8963.whoami
	ak8963inspi_read(fd, 0, buf, 1);
	if(0x48 != buf[0]){
		printf("ak8963.whoami != 0x48\n");
		return 0;
	}


//init mpu9250


//init ak8963
	buf[0] = 1;
	ak8963inspi_write(fd, 0xb, buf, 1);

	buf[0] = 0x16;
	ak8963inspi_write(fd, 0xa, buf, 1);


//debug print
	//mpu9250: read must | 0x80
	systemspi_read(fd, 0|0x80, buf, 128);
	for(j=0;j<128;j++){
		printf("%02x ", buf[j]);
		if((j%16) == 15)printf("\n");
	}


//start loop
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

		printf(
			"acc:	%d	%d	%d\n"
			"gyr:	%d	%d	%d\n"
			"mag:	%d	%d	%d\n",
			acc[0], acc[1], acc[2],
			gyr[0], gyr[1], gyr[2],
			mag[0], mag[1], mag[2]
		);
	}

	SpiClosePort(0);
	return 0;
}
