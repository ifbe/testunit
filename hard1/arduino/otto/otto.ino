#include <Wire.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;


//hcsr04
const int trigpin = 32;
const int echopin = 33;
long duration;
long distance;

//pca9685
int val = 290;




void setup()
{
  int err;
  Serial.begin(115200);
  Serial.println("@serial ok!");


  SerialBT.begin("ESP32test");
  Serial.println("serialbt ok!");


  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);


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

    val = atoi((char*)c);
  }
  if(SerialBT.available()){
    j = SerialBT.readBytesUntil('\n', c, 5);
    c[j] = 0;

    val = atoi((char*)c);
  }
  Serial.println(val);


//1: set values
  Wire.beginTransmission(0x40);
  Wire.write(0x6);

  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);

  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);

  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);
 
  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);

  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);

  Wire.write(0x0);
  Wire.write(0x0);
  Wire.write(val & 0xff);
  Wire.write(val >> 8);

  Wire.endTransmission();  
  delay(1);


//2.sr04
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distance = duration/2*340/1000;

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println("mm");


//3.debug print
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
