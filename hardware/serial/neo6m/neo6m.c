#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<termios.h>



int fp=-1;
unsigned char buffer[0x1000];



void settty(int this)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);

	/* Error Handling */
	if ( tcgetattr ( this, &tty ) != 0 )
	{
		printf("@tcgetattr:%d,%s\n", errno, strerror(errno) );
	}

	/* Set Baud Rate */
	cfsetospeed (&tty, B9600);
	cfsetispeed (&tty, B9600);
	//cfsetospeed (&tty, B38400);
	//cfsetispeed (&tty, B38400);

	// Setting other Port Stuff
	tty.c_cflag     &=  ~PARENB;        // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;
	tty.c_cflag     &=  ~CRTSCTS;       // no flow control
	tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
	tty.c_oflag     =   0;                  // no remapping, no delays
	tty.c_cc[VMIN]      =   0;                  // read doesn't block
	tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
	tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
	tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	tty.c_oflag     &=  ~OPOST;              // make raw

	// Flush Port, then applies attributes
	tcflush( this, TCIFLUSH );

	if ( tcsetattr ( this, TCSANOW, &tty ) != 0)
	{
		printf("@tcsetattr:%d,%s\n", errno, strerror(errno) );
	}
}

void main()
{
	int ret;
	int where;

	fp=open("/dev/ttyS0",O_RDWR);
	if(fp<0)return;

	settty(fp);

	where=0;
	while(1)
	{
		//read
		ret=read(fp,buffer+where,1);
		if(ret<=0)continue;

		//found
		if(buffer[where]==0xa)
		{
			buffer[where+1]=0;
			where=0;

			printf("%s\n",buffer);
			continue;
		}

		//next
		where++;
		if(where>0x800)
		{
			where=0;
			printf("read error\n");
			continue;
		}

	}
}
