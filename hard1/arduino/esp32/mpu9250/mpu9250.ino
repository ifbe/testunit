#include <Wire.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include "AsyncUDP.h"
#include "BluetoothSerial.h"
#define thisname "mpu9250"

//bt
BluetoothSerial SerialBT;

//i'm accesspoint
const char* ap_ssid = thisname;
const char* ap_pass = "88888888";
char ap_stat = 0;

//i'm station
const char* sta_ssid = "********";
const char* sta_pass = "********";
char sta_stat = 0;

//udp server
AsyncUDP udp;
int port = 1234;

//tcp server
WebServer server(80);

//0=250dps, 1=500dps, 2=1000dps, 3=2000dps
#define gyr_cfg 2
#define gyr_max 1000

//0=2g, 1=4g, 2=8g, 3=16g
#define acc_cfg 0
#define acc_max 2*9.8

//
#define halfT 0.005
#define Kp 2.0
#define Ki 0.005
static long prevtime;
static long currtime;
static float measure[10];

//????
static float integralx;
static float integraly;
static float integralz;

//quaternion
static float qw;
static float qx;
static float qy;
static float qz;




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
  
  
  //normalize a
  norm = 1.0 / sqrt(ax*ax+ay*ay+az*az);
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;
  
  
  //
  vx = 2*(qx*qz - qw*qy);
  vy = 2*(qw*qx + qy*qz);
  vz = qw*qw - qx*qx - qy*qy + qz*qz;
  
  ex = vy*az - vz*ay;
  ey = vz*ax - vx*az;
  ez = vx*ay - vy*ax;
  
  integralx += ex*Ki;
  integraly += ey*Ki;
  integralz += ez*Ki;
  
  gx = gx + Kp*ex + integralx;
  gy = gy + Kp*ey + integraly;
  gz = gz + Kp*ez + integralz;
  
  qw = qw + (-qx*gx - qy*gy - qz*gz)*halfT;
  qx = qx + (qw*gx + qy*gz - qz*gy)*halfT;
  qy = qy + (qw*gy - qx*gz + qz*gx)*halfT;
  qz = qz + (qw*gz + qx*gy - qy*gx)*halfT;
  
  norm = 1.0 / sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
  qw = qw * norm;
  qx = qx * norm;
  qy = qy * norm;
  qz = qz * norm;


  vx = atan2(2*(qw*qx+qy*qz),1-2*(qx*qx+qy*qy))*180/3.141592653;
  vy = asin(2*qw*qy - 2*qx*qz)*180/3.141592653;
  vz = atan2(2*(qw*qz+qx*qy),1-2*(qy*qy+qz*qz))*180/3.141592653;

  Serial.print(vx);
  Serial.print(',');

  Serial.print(vy);
  Serial.print(',');

  Serial.print(vz);
  Serial.print('\n');
}
void readvalue()
{
  char j;
  long tmp;
  unsigned char buf[32];

  Wire.beginTransmission(0x68);
  Wire.write(0x3b);
  Wire.endTransmission();

  j = 0;
  Wire.requestFrom(0x68, 14);
  while (Wire.available())
  {
    buf[j] = Wire.read();
    j++;
  }
/*
  for(j=0;j<7;j++){
    tmp = buf[j*2+0];
    buf[j*2+0] = buf[j*2+1];
    buf[j*2+1] = tmp;

    tmp = *(short*)(buf+j*2);
    Serial.print(tmp);
    Serial.print(',');
  }
  Serial.print('\n');*/

  for(j=0;j<7;j++){
    tmp = buf[j*2+0];
    buf[j*2+0] = buf[j*2+1];
    buf[j*2+1] = tmp;
  }

  tmp = *(short*)(buf+8);
  measure[0] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = *(short*)(buf+10);
  measure[1] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = *(short*)(buf+12);
  measure[2] = tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = *(short*)(buf+0);
  measure[3] =-tmp * acc_max / 32768.0;

  tmp = *(short*)(buf+2);
  measure[4] =-tmp * acc_max / 32768.0;

  tmp = *(short*)(buf+4);
  measure[5] =-tmp * acc_max / 32768.0;
/*
  Serial.print(measure[0]);
  Serial.print(',');
  Serial.print(measure[1]);
  Serial.print(',');
  Serial.print(measure[2]);
  Serial.print(',');

  Serial.print(measure[3]);
  Serial.print(',');
  Serial.print(measure[4]);
  Serial.print(',');
  Serial.print(measure[5]);
  Serial.print('\n');*/
}
void loop()
{
  char j;
  unsigned char c[32];

//0: get command
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

  if(WL_CONNECTED == WiFi.status()){
    if(0 == sta_stat){
      sta_stat = 1;
      Serial.print("sta.ip: ");
      Serial.println(WiFi.localIP());
    }
  }

  do{
    currtime = millis();
  }while(currtime - prevtime < 10);
  //Serial.println(currtime - prevtime);
  prevtime = currtime;

  server.handleClient();

  readvalue();

  imuupdate(measure[0],measure[1],measure[2],measure[3],measure[4],measure[5]);
}

void handleRoot() {
  char buf[32];
  String message;
  //digitalWrite(LED_BUILTIN, 1);

  message  = dtostrf(qw, 3, 3, buf);
  message += ',';
  message += dtostrf(qx, 3, 3, buf);
  message += ',';
  message += dtostrf(qy, 3, 3, buf);
  message += ',';
  message += dtostrf(qz, 3, 3, buf);
  message += '\n';

  server.send(200, "text/plain", message);
  //digitalWrite(LED_BUILTIN, 0);
}
void handleNotFound() {
  //digitalWrite(LED_BUILTIN, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(LED_BUILTIN, 0);
}





void init_serial()
{
  Serial.begin(115200);
  Serial.println("@serial ok!");
}
void init_bluetooth()
{
  SerialBT.begin(thisname);
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
void init_tcpserver(){
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
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
  qw = 1.0;
  qx = qy = qz = 0.0;
  integralx = integraly = integralz = 0.0;
}
void setup()
{
  //pinMode(LED_BUILTIN, OUTPUT);
  init_serial();

  init_bluetooth();
  init_wifi();

  init_udpserver();
  init_tcpserver();

  init_i2c();
  init_var();

  prevtime = millis();
}
