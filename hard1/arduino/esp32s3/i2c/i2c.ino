#include<Wire.h>
byte busStatus;

void setup()
{
  Serial.begin(115200);
  Wire.begin(18, 17);  //SCL = 17, SDA = 18
}

void loop()
{
  Serial.println("loop");
  for (int i2cAddress = 0x00; i2cAddress < 0x80; i2cAddress++)
  {
    Wire.beginTransmission(i2cAddress);
    busStatus = Wire.endTransmission();
    if (busStatus == 0x00)
    {
      Serial.print("I2C Device found at address: 0x");
      Serial.println(i2cAddress, HEX);
    }

    else
    {
      Serial.print("I2C Device not found at address: 0x");
      Serial.println(i2cAddress, HEX);
    }
  }

  delay(1000);
}
