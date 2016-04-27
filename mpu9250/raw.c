#define BYTE unsigned char
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>
int fp;
unsigned char sequence[20][2]=
{
	// Reset Device
	{0x80,0x6b},		//MPUREG_PWR_MGMT_1},

	// Clock Source
	{0x01,0x6b},		//MPUREG_PWR_MGMT_1},

	// Enable Acc & Gyro
	{0x00,0x6c},		//MPUREG_PWR_MGMT_2},

	// Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth     188Hz
	{0x9,0x1d},		//MPUREG_CONFIG},

	// +-2000dps
	{0x18,0x1b},		//MPUREG_GYRO_CONFIG},

	// +-4G
	{0x08,0x1c},		//MPUREG_ACCEL_CONFIG},

	// Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
	{0x09,0x1d},		//MPUREG_ACCEL_CONFIG_2},

	{0x30,0x37},		//MPUREG_INT_PIN_CFG},

        //{0x40, MPUREG_I2C_MST_CTRL},   // I2C Speed 348 kHz
        //{0x20, MPUREG_USER_CTRL},      // Enable AUX

	// I2C Master mode
	{0x20,0x6a},		//MPUREG_USER_CTRL},

	//I2C configuration multi-master  IIC 400KHz
	{0x0D,0x24},		//MPUREG_I2C_MST_CTRL},

	//Set the I2C slave address of AK8963 and set for write.
	{0xc,0x25},		//MPUREG_I2C_SLV0_ADDR},

        //{0x09, MPUREG_I2C_SLV4_CTRL},
        //{0x81, MPUREG_I2C_MST_DELAY_CTRL}, //Enable I2C delay

	//I2C slave 0 register address from where to begin data transfer
	{0xb,0x26},		//MPUREG_I2C_SLV0_REG},

	// Reset AK8963
	{0x01,0x63},		//MPUREG_I2C_SLV0_DO},

	//Enable I2C and set 1 byte
	{0x81,0x27},		//MPUREG_I2C_SLV0_CTRL},

	//I2C slave 0 register address from where to begin data transfer
	{0xa,0x26},		//MPUREG_I2C_SLV0_REG},

	// Register value to continuous measurement in 16bit
	{0x12,0x63},		//MPUREG_I2C_SLV0_DO},

	//Enable I2C and set 1 byte
	{0x81,0x27},		//MPUREG_I2C_SLV0_CTRL},

	{0,0}
};

int systemi2c_write(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
        int ret;

        //select device
        ret=ioctl(fp,I2C_SLAVE,dev);
        if(ret < 0)
        {
                printf("error@systemi2c_write.ioctl\n");
                return;
        }

        //select register
        ret=write(fp,&reg,1);
        if(ret!=1)
        {
                printf("error@systemi2c_write.writereg:%x\n",ret);
                return -1;
        }

        //write data
        ret=write(fp,buf,count);
        if(ret<=0)
        {
                printf("error@systemi2c_write.writedata:%x\n",ret);
                return -2;
        }
        return 1;
}
int systemi2c_read(BYTE dev,BYTE reg,BYTE* buf,BYTE count)
{
        int ret;

        //select device
        ret=ioctl(fp,I2C_SLAVE,dev);
        if(ret < 0)
        {
                printf("error@systemi2c_read.ioctl\n");
                return;
        }

        //select register
        ret=write(fp,&reg,1);
        if(ret!=1)
        {
                printf("error@systemi2c_read.writereg:%x\n",ret);
                return -1;
        }

        //read data
        ret=read(fp,buf,count);
        if(ret<=0)
        {
                printf("error@systemi2c_read.readdata:%x\n",ret);
                return -2;
        }
        return 1;
}

void initmpu9250()
{
	int i=0;
	for(i=0;i<32;i++)
	{
		if(sequence[i][1]==0)break;

		systemi2c_write(0x68,sequence[i][1],&sequence[i][0],1);

		usleep(10*1000);
	}
}

void main()
{
        int ret;
        unsigned char buf[18];

        //open
        fp = open("/dev/i2c-1",O_RDWR);
        if(fp<0)
        {
                printf("error open\n");
                return;
        }

	initmpu9250();
	while(1)
        {
                //device,register,memory,count
                systemi2c_read(0x68,0x3b,buf+0,14);

                printf("(%.5d,%.5d,%.5d),       ",
                        (signed short)((buf[0]<<8)|buf[1]),
                        (signed short)((buf[2]<<8)|buf[3]),
                        (signed short)((buf[4]<<8)|buf[5])
                );
                printf("(%.5d,%.5d,%.5d)\n",
                        (signed short)((buf[8]<<8)|buf[9]),
                        (signed short)((buf[10]<<8)|buf[11]),
                        (signed short)((buf[12]<<8)|buf[13])
                );
        }
}

