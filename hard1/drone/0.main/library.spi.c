#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static int fp;
struct spi_ioc_transfer xfer[2];

char buf[10];
char buf2[10];




int systemspi_init() {
        __u8  lsb = 0;
        __u8 mode = 0;
        __u8 bits = 8;
        __u32 speed = 10000000;

        if ((fp = open("/dev/spidev0.0",O_RDWR)) < 0) {
                printf("Failed to open the bus.");
                exit(1);
        }

        if (ioctl(fp, SPI_IOC_WR_MODE, &mode)<0) {
                perror("can't set spi mode");
                return;
        }

        if (ioctl(fp, SPI_IOC_RD_MODE, &mode) < 0) {
                perror("SPI rd_mode");
                return;
        }
        if (ioctl(fp, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
                perror("SPI rd_lsb_fist");
                return;
        }
        if (ioctl(fp, SPI_IOC_WR_BITS_PER_WORD, &bits)<0) {
                perror("can't set bits per word");
                return;
        }
        if (ioctl(fp, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
                perror("SPI bits_per_word");
                return;
        }
        if (ioctl(fp, SPI_IOC_WR_MAX_SPEED_HZ, &speed)<0) {
                perror("can't set max speed hz");
                return;
        }
        if (ioctl(fp, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
                perror("SPI max_speed_hz");
                return;
        }
        return fp;
}

void systemspi_kill()
{
	close(fp);
}

char * spi_read(int addr,int nbytes) {
        int status;

        memset(buf, 0, sizeof buf);
        memset(buf2, 0, sizeof buf2);
        buf[0] = addr | 128;
        xfer[0].tx_buf = (unsigned long)buf;
        xfer[0].len = 1; 
        xfer[1].rx_buf = (unsigned long) buf2;
        xfer[1].len = nbytes; 
        status = ioctl(fp, SPI_IOC_MESSAGE(2), xfer);
        if (status < 0) {
                perror("SPI_IOC_MESSAGE");
                return;
        }
        return buf2;
}

void spi_write(int addr, char value) {
        int status;

        memset(buf, 0, sizeof buf);
        buf[0] = addr & 127;
        buf[1] = value;
        xfer[0].tx_buf = (unsigned long)buf;
        xfer[0].len = 2;
        status = ioctl(fp, SPI_IOC_MESSAGE(1), xfer);
        if (status < 0) {
                perror("SPI_IOC_MESSAGE");
        }
}
