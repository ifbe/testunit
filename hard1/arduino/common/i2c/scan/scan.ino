#include <Wire.h>
void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Serial.println("14CORE | i2C SCANNER ");
  Serial.println("=========================");
  Serial.println("Starting ....");
  Serial.println("");
}

void loop()
{
  byte error, address;
  int devices;

  Serial.println("Scanning...");

  devices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address | 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      devices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknow error at address | 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (devices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000); 
}
