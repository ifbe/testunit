#include <Wire.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "AsyncUDP.h"
#include "BluetoothSerial.h"

//bt
BluetoothSerial SerialBT;

//i'm accesspoint
const char* ap_ssid = "otto";
const char* ap_pass = "88888888";
char ap_stat = 0;

//i'm station
const char* sta_ssid = "********";
const char* sta_pass = "********";
char sta_stat = 0;

//udp server
AsyncUDP udp;
int port = 1234;

//hcsr04
const int trigpin = 32;
const int echopin = 33;
long duration;
long distance;

//pca9685
int val[6] = {
  280,    //arm_r: 490=up, 75=down
  290,    //arm_l: 85=up, 485=down
  300,    //leg_r: 110=in, 480=out
  275,    //leg_l: 100=out, 470=in
  285,    //foot_r:100=in, 480=out
  300     //foot_l:110=out, 480=in
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

  for(j=0;j<6;j++){
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


//2.sr04
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distance = duration/2*340/1000;

  if(distance < 1000){
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.println("mm");
  }
/*
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
*/
}




void init_serial()
{
  Serial.begin(115200);
  Serial.println("@serial ok!");
}
void init_bluetooth()
{
  SerialBT.begin("otto");
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
void init_hcsr04()
{
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
}
void init_i2c()
{
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

  init_hcsr04();
  init_i2c();
}
