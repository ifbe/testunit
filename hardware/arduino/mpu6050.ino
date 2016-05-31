// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
    unsigned char c;
    Serial.begin(115200);  // start serial for output
    Wire.begin();        // join i2c bus (address optional for master)
    
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
    Wire.requestFrom(0x68, 14);    // request 6 bytes from slave device #2
    while (Wire.available())   // slave may send less than requested
    {
        c[i] = Wire.read(); // receive a byte as character
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
