#include <Wire.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//[0,5]=left, [6,11]=right
//[0,1]=near, [2,3]=mid, [4,5]=front, [6,7]=front, [8,9]=mid, [10,11]=near
//{0,2,4,6,8,10}=leg, {1,3,5,7,9,11}=foot
int val[12] = {
  290,290,290,290,
  290,290,290,290,
  290,290,290,290
};




void setup()
{
  int err;
  Serial.begin(115200);
  Serial.println("@serial ok!");


  SerialBT.begin("ESP32test");
  Serial.println("serialbt ok!");


  Wire.begin();

  //sleep
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.write(0x31);
  Wire.endTransmission();
  delay(1);

  //prescale
  Wire.beginTransmission(0x40);
  Wire.write(0xfe);
  Wire.write(132);
  Wire.endTransmission();  
  delay(1);

  //wakeup
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.write(0xa1);
  Wire.endTransmission();  
  delay(1);

  //restart
  Wire.beginTransmission(0x40);
  Wire.write(0x01);
  Wire.write(0x04);
  Wire.endTransmission();  
  delay(1);
}

void loop()
{
  char j;
  int err;
  unsigned char c[32];

//0: get command
  if(Serial.available()){
    j = Serial.readBytesUntil('\n', c, 5);
    c[j] = 0;

    j = c[0]-'a';
    if((j>=0)&&(j<12))val[j] = atoi((char*)(c+1));
  }
  if(SerialBT.available()){
    j = SerialBT.readBytesUntil('\n', c, 5);
    c[j] = 0;

    if((j>=0)&&(j<12))val[j] = atoi((char*)(c+1));
  }


//1: set values
  Wire.beginTransmission(0x40);
  Wire.write(0x6);

  for(j=0;j<12;j++){
    Serial.print(val[j]);
    Serial.print(' ');

    Wire.write(0x0);
    Wire.write(0x0);
    Wire.write(val[j] & 0xff);
    Wire.write(val[j] >> 8);
  }
  Serial.println();

  Wire.endTransmission();  
  delay(1);


//2.debug print
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  err = Wire.endTransmission();
  if ( err )
  {
    Serial.print("endTransmission = ");
    Serial.println(err);
    return;
  }

  Wire.requestFrom(0x40, 32);
  delay(1);

  err = Wire.available();
  if(32 == err){
    for(j=0;j<32;j++){
      c[j] = Wire.read();
      Serial.print(c[j], HEX);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  else{
    Serial.print("available = ");
    Serial.println(err);
  }
}
