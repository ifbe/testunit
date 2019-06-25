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




int mfrc522_read(int reg)
{
	return systemspi_read_byte(fd, ((reg<<1)&0x7e)|0x80);
}
int mfrc522_write(int reg, int val)
{
	return systemspi_write_byte(fd, (reg<<1)&0x7e, val);
}




int main(int argc, char** argv)
{
	int j;
	u8 buf[256];
	SpiOpenPort("/dev/spidev0.0");


	for(j=0;j<256;j++){
		printf("%02x ", mfrc522_read(j));
		if((j%16) == 15)printf("\n");
	}


	SpiClosePort(0);
	return 0;
}
