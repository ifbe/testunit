#include <Wire.h>

void setup()
{
	Serial.begin(115200);
	Mouse.begin();
	Wire.begin();
}

void loop()
{
	int x,y,z;
	int c[4];
  
  
  
  
	Wire.beginTransmission(0x48);
	Wire.write(0x42);
	Wire.endTransmission();
	Wire.requestFrom(0x48, 2);
	while (Wire.available())
	{
		c[0] = Wire.read();
		c[0] = Wire.read();
	}




	Wire.beginTransmission(0x48);
	Wire.write(0x43);
	Wire.endTransmission();
	Wire.requestFrom(0x48, 2);
	while (Wire.available())
	{
		c[1] = Wire.read();
		c[1] = Wire.read();
	}



	Wire.beginTransmission(0x48);
	Wire.write(0x44);
	Wire.endTransmission();
	Wire.requestFrom(0x48, 2);
	while (Wire.available())
	{
		c[2] = Wire.read();
		c[2] = Wire.read();
	}




	Wire.beginTransmission(0x48);
	Wire.write(0x45);
	Wire.endTransmission();
	Wire.requestFrom(0x48, 2);
	while (Wire.available())
	{
		c[3] = Wire.read();
		c[3] = Wire.read();
	}



	c[0] = c[0] - 0x80;
	c[1] = c[1] - 0x80;
	c[2] = 0x80 - c[2];
	c[3] = 0x80 - c[3];



	x=y=z=0;
	if( (c[0]<-16) | (c[0]>16) )
	{
		x = c[0]>>5;
	}
	if( (c[1]<-16) | (c[1]>16) )
	{
		y = -(c[1]>>5);
	}
	if( (c[3]<-16) | (c[3]>16) )
	{
		z = c[3]>>7;
	}

	Mouse.move(x, y, z);
	//delay(2);



	/*
	Serial.print( c[0] );
	Serial.print(' ');
	Serial.print( c[1] );
	Serial.print(' ');
	Serial.print( c[2] );
	Serial.print(' ');
	Serial.print( c[3] );
	Serial.print(' ');
	Serial.println("");
	*/
}
