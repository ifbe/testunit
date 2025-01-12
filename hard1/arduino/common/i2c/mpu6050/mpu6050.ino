#include <Wire.h>

void setup()
{
	unsigned char c;
	Serial.begin(115200);
	Wire.begin();

	Wire.beginTransmission(0x68);
	Wire.write(0x6b);
	Wire.write(0x80);
	Wire.endTransmission();  
	delay(1);
  
	Wire.beginTransmission(0x68);
	Wire.write(0x6b);
	Wire.write(0x01);
	Wire.endTransmission();
	delay(1);

	Wire.beginTransmission(0x68);
	Wire.write(0x6c);
	Wire.write(0x00);
	Wire.endTransmission();
	delay(1);

	Wire.beginTransmission(0x68);
	Wire.write(0x1a);
	Wire.write(0x03);
	Wire.endTransmission();
	delay(1);

	Wire.beginTransmission(0x68);
	Wire.write(0x19);
	Wire.write(0x04);
	Wire.endTransmission();
	delay(1);




	Wire.beginTransmission(0x68);
	Wire.write(0x1b);
	Wire.endTransmission();
	Wire.requestFrom(0x68, 1);
	while(Wire.available())
	{
		c = Wire.read();
	}
	delay(1);

	Wire.beginTransmission(0x68);
	Wire.write(0x1b);
	Wire.write(c & (0xe5) | (3<<3));
	Wire.endTransmission();
	delay(1);




	Wire.beginTransmission(0x68);
	Wire.write(0x1c);
	Wire.endTransmission();
	Wire.requestFrom(0x68, 1);
	while(Wire.available())
	{
		c = Wire.read();
	}

	Wire.beginTransmission(0x68);
	Wire.write(0x1c);
	Wire.write(c & (0xe7) | (0<<3));
	Wire.endTransmission();
	delay(1);




	Wire.beginTransmission(0x68);
	Wire.write(0x1d);
	Wire.endTransmission();
	Wire.requestFrom(0x68, 1);
	while(Wire.available())
	{
		c = Wire.read();
	}

	Wire.beginTransmission(0x68);
	Wire.write(0x1d);
	Wire.write(c & 0xf0 | 0x3);
	Wire.endTransmission();
	delay(1);
}

void loop()
{
	char i;
	unsigned char c[14];
	long temp;
  
	Wire.beginTransmission(0x68);
	Wire.write(0x3b);
	Wire.endTransmission();

	i=0;
	Wire.requestFrom(0x68, 14);
	while (Wire.available())
	{
		c[i] = Wire.read();
		i++;
	}


	temp=(c[0]<<8) + c[1];
	Serial.print( temp );
	Serial.print(' ');

	temp=(c[2]<<8) + c[3];
	Serial.print( temp );
	Serial.print(' ');

	temp=(c[4]<<8) + c[5];
	Serial.print( temp );
	Serial.print(' ');

	temp=(c[8]<<8) + c[9];
	Serial.print( temp );
	Serial.print(' ');

	temp=(c[10]<<8) + c[11];
	Serial.print( temp );
	Serial.print(' ');

	temp=(c[12]<<8) + c[13];
	Serial.print( temp );
	Serial.print(' ');

	Serial.println("");
}
