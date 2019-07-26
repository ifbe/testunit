#include <Wire.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "AsyncUDP.h"
#include "BluetoothSerial.h"

//bt
BluetoothSerial SerialBT;

//i'm accesspoint
const char* ap_ssid = "balance";
const char* ap_pass = "88888888";
char ap_stat = 0;

//i'm station
const char* sta_ssid = "********";
const char* sta_pass = "********";
char sta_stat = 0;

//udp server
AsyncUDP udp;
int port = 1234;

//0=250dps, 1=500dps, 2=1000dps, 3=2000dps
#define gyr_cfg 2
#define gyr_max 1000

//0=2g, 1=4g, 2=8g, 3=16g
#define acc_cfg 0
#define acc_max 2*9.8

//
#define halfT 0.001
#define Kp 100.0f
#define Ki 0.005f

static float measure[10];

//????
static float integralx;
static float integraly;
static float integralz;

//quaternion
static float q0;
static float q1;
static float q2;
static float q3;




void otto_parse(unsigned char* buf, int len)
{
  int j = buf[0]-'a';
  if((j>=0)&&(j<12))j = atoi((char*)(buf+1));
}
void imuupdate(
  float gx, float gy, float gz,
  float ax, float ay, float az)
{
  float norm;
  float vx, vy, vz;
  float ex, ey, ez;
  float tx, ty, tz;
  
  
  //normalize a
  norm = sqrt(ax*ax+ay*ay+az*az);
  ax = ax/norm;
  ay = ay/norm;
  az = az/norm;
  
  
  //
  vx = 2*(q1*q3 - q0*q2);
  vy = 2*(q0*q1 + q2*q3);
  vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
  
  ex = (ay*vz - az*vy);
  ey = (az*vx - ax*vz);
  ez = (ax*vy - ay*vx);
  
  integralx += ex*Ki;
  integraly += ey*Ki;
  integralz += ez*Ki;
  
  gx = gx + Kp*ex + integralx;
  gy = gy + Kp*ey + integraly;
  gz = gz + Kp*ez + integralz;
  
  q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;
  
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 = q0 / norm;
  q1 = q1 / norm;
  q2 = q2 / norm;
  q3 = q3 / norm;

  tx = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
  ty = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;
  tz = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;

  Serial.print(tx);
  Serial.print(' ');

  Serial.print(ty);
  Serial.print(' ');

  Serial.print(tz);
  Serial.print(' ');

  Serial.println();
}
void readvalue()
{
  char j;
  long tmp;
  unsigned char c[32];

  Wire.beginTransmission(0x68);
  Wire.write(0x3b);
  Wire.endTransmission();

  j = 0;
  Wire.requestFrom(0x68, 14);
  while (Wire.available())
  {
    c[j] = Wire.read();
    j++;
  }


  tmp = (c[8]<<8) + c[9];
  if(tmp > 32767)tmp -= 0x10000;
  measure[0] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = (c[10]<<8) + c[11];
  if(tmp > 32767)tmp -= 0x10000;
  measure[1] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = (c[12]<<8) + c[13];
  if(tmp > 32767)tmp -= 0x10000;
  measure[2] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = (c[0]<<8) + c[1];
  if(tmp > 32767)tmp -= 0x10000;
  measure[3] = tmp * acc_max / 32768.0;

  tmp = (c[2]<<8) + c[3];
  if(tmp > 32767)tmp -= 0x10000;
  measure[4] = tmp * acc_max / 32768.0;

  tmp = (c[4]<<8) + c[5];
  if(tmp > 32767)tmp -= 0x10000;
  measure[5] = tmp * acc_max / 32768.0;
/*
  Serial.print(measure[0]);
  Serial.print(' ');
  Serial.print(measure[1]);
  Serial.print(' ');
  Serial.print(measure[2]);
  Serial.print(' ');

  Serial.print(measure[3]);
  Serial.print(' ');
  Serial.print(measure[4]);
  Serial.print(' ');
  Serial.print(measure[5]);
  Serial.print(' ');

  Serial.println("");
*/
}
void loop()
{
  char j;
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

  readvalue();

  imuupdate(measure[0],measure[1],measure[2],measure[3],measure[4],measure[5]);
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
  unsigned char c;
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
  Wire.beginTransmission(0x68);
  Wire.write(0x1b);
  Wire.write(c & (0xe7) | (gyr_cfg<<3));
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
  Wire.write(c & (0xe7) | (acc_cfg<<3));
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
void init_var()
{
  q0 = 1.0;
  q1 = q2 = q3 = 0.0;
  integralx = integraly = integralz = 0.0;
}
void setup()
{
  init_serial();
  init_bluetooth();

  init_wifi();
  init_udpserver();

  init_i2c();
  init_var();
}
