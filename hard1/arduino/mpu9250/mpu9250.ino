// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup() {
  Serial.begin(115200);  // start serial for output

  Wire.begin();        // join i2c bus (address optional for master)

  Wire.beginTransmission(0x68);
  Wire.write(0x6b);
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop() {

  //mpu6050
  Wire.beginTransmission(0x68);
  Wire.write(0x3b);
  Wire.requestFrom(0x68, 14, true);
  Wire.endTransmission(true);

  while (Wire.available()) {
    unsigned char j = Wire.read();
    unsigned char k = Wire.read();

    Serial.print((j<<8)+k);
    Serial.print(' ');
  }
  Serial.print('\n');

  //delay(1000);
}
