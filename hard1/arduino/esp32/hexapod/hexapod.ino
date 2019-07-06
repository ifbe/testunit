#include <Wire.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "AsyncUDP.h"
#include "BluetoothSerial.h"

//bt
BluetoothSerial SerialBT;

//i'm accesspoint
const char* ap_ssid = "hexapod";
const char* ap_pass = "88888888";
char ap_stat = 0;

//i'm station
const char* sta_ssid = "********";
const char* sta_pass = "********";
char sta_stat = 0;

//udp server
AsyncUDP udp;
int port = 1234;

//[0,5]=left, [6,11]=right
//[0,1]=near, [2,3]=mid, [4,5]=front, [6,7]=front, [8,9]=mid, [10,11]=near
//{0,2,4,6,8,10}=leg, {1,3,5,7,9,11}=foot
int val[12] = {
  290,290,290,290,
  290,290,290,290,
  290,290,290,290
};




void otto_parse(unsigned char* buf, int len)
{
  int j = buf[0]-'a';
  if((j>=0)&&(j<12))val[j] = atoi((char*)(buf+1));
}
void loop()
{
  char j;
  int err;
  unsigned char c[32];

//0: get command
  if(WL_CONNECTED == WiFi.status()){
    if(0 == sta_stat){
      sta_stat = 1;
      Serial.print("sta.ip: ");
      Serial.println(WiFi.localIP());
    }
  }
  if(Serial.available()){
    j = Serial.readBytesUntil('\n', c, 5);
    c[j] = 0;
    otto_parse(c, j);
  }

  if(SerialBT.available()){
    j = SerialBT.readBytesUntil('\n', c, 5);
    c[j] = 0;
    otto_parse(c, j);
  }


//1: set values
  Wire.beginTransmission(0x40);
  Wire.write(0x6);

  for(j=0;j<12;j++){
    //Serial.print(val[j]);
    //Serial.print(' ');

    Wire.write(0x0);
    Wire.write(0x0);
    Wire.write(val[j] & 0xff);
    Wire.write(val[j] >> 8);
  }
  //Serial.println();

  Wire.endTransmission();
  delay(1);

/*
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
*/
}




void init_serial()
{
  Serial.begin(115200);
  Serial.println("@serial ok!");
}
void init_bluetooth()
{
  SerialBT.begin("hexapod");
  Serial.println("serialbt ok!");
}
void init_wifi()
{
  WiFi.mode(WIFI_AP_STA);

  //access point part
  Serial.println("acting as Accesspoint...");
  WiFi.softAP(ap_ssid, ap_pass, 7, 0, 5);

  Serial.print("ap.ip: ");
  Serial.println(WiFi.softAPIP());

  //station part
  Serial.println("acting as station...");
  WiFi.begin(sta_ssid, sta_pass);
}
void init_udpserver()
{
  udp.listen(port);

  Serial.print("UDP Listening...");
  Serial.println(WiFi.localIP());

  udp.onPacket([](AsyncUDPPacket packet) {
    //debug print
    Serial.print("UDP Packet Type: ");
    Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
    Serial.print(", From: ");
    Serial.print(packet.remoteIP());
    Serial.print(":");
    Serial.print(packet.remotePort());
    Serial.print(", To: ");
    Serial.print(packet.localIP());
    Serial.print(":");
    Serial.print(packet.localPort());
    Serial.print(", Length: ");
    Serial.print(packet.length());
    Serial.print(", Data: ");
    Serial.write(packet.data(), packet.length());

    //reply to the client
    packet.printf("Got %u bytes of data\n", packet.length());

    //
    otto_parse(packet.data(), packet.length());
  });
}
void init_i2c()
{
  //i2c begin
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
void setup()
{
  init_serial();
  init_bluetooth();

  init_wifi();
  init_udpserver();

  init_i2c();
}
