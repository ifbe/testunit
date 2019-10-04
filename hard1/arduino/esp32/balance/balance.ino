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
const char* sta_ssid = "Tenda_1F34E0";
const char* sta_pass = "52755227";
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
#define Kp 20.0f
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

//eulerian
static float eux;
static float euy;
static float euz;

//pwm
//#define LEDC_BASE_FREQ     31280
#define CH0   0
#define CH1   1

#define EN_L    14
#define DIR_L   12
#define PWM_L   13

#define EN_R    16
#define DIR_R   4
#define PWM_R   2

float PID_P = 500.0;
float PID_I = 0.0;
float PID_D = 0.5;
float error_0 = 0.0;
float error_1 = 0.0;
float error_2 = 0.0;
int dir = 1;
int ena = 0;
int out = 0;

int last = 0;
int curr = 0;




void otto_parse(unsigned char* buf, int len)
{
  int j = buf[0]-'a';
  if((j>=0)&&(j<12))j = atoi((char*)(buf+1));
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
  measure[1] = -tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = (c[12]<<8) + c[13];
  if(tmp > 32767)tmp -= 0x10000;
  measure[2] = -tmp * gyr_max / 32768.0 * PI / 180.0;

  tmp = (c[0]<<8) + c[1];
  if(tmp > 32767)tmp -= 0x10000;
  measure[3] = tmp * acc_max / 32768.0;

  tmp = (c[2]<<8) + c[3];
  if(tmp > 32767)tmp -= 0x10000;
  measure[4] = -tmp * acc_max / 32768.0;

  tmp = (c[4]<<8) + c[5];
  if(tmp > 32767)tmp -= 0x10000;
  measure[5] = -tmp * acc_max / 32768.0;
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
  Serial.print('\n');
*/
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

  eux = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))*180/3.141592653;
  euy = asin(2*q0*q2 - 2*q1*q3)*180/3.141592653;
  euz = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))*180/3.141592653;
/*
  Serial.print(eux);
  Serial.print(',');

  Serial.print(euy);
  Serial.print(',');

  Serial.print(euz);
  Serial.print('\n');
*/
}
void pidupdate()
{
  error_0 = eux - 0;
  out = PID_P * error_0
      + PID_D * (error_0 - error_1);

  error_2 = error_1;
  error_1 = error_0;
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

  server.handleClient();

  readvalue();

  imuupdate(measure[0],measure[1],measure[2],measure[3],measure[4],measure[5]);

  pidupdate();

  ena = 1;
  if( (eux > -45) && (eux < -1) )ena = 0;
  if( (eux >   1) && (eux < 45) )ena = 0;
  digitalWrite(EN_L, ena);
  digitalWrite(EN_R, ena);

  if(eux < 0.0)dir = 1;
  else dir = 0;
  digitalWrite(DIR_L, dir);
  digitalWrite(DIR_R, dir);

  if(out < 0)out = -out;
  if(out < 2)out = 2;
  if(out > 200*16)out = 200*16;
  ledcSetup(CH0, out, 14);
  ledcWrite(CH0, 8192);
  ledcSetup(CH1, out, 14);
  ledcWrite(CH1, 8192);

  Serial.print(ena);
  Serial.print(',');
  Serial.print(dir);
  Serial.print(',');
  Serial.print(out);
  Serial.print(',');
  Serial.println(curr-last);

  last = curr;
  do{
    curr = micros();
  }while(curr-last < 10000);
}

void handleRoot() {
  char buf[32];
  String msg;
  //digitalWrite(LED_BUILTIN, 1);

  msg = "<html>";

  msg += "ex: ";
  msg += dtostrf(eux, 3, 3, buf);
  msg += "<br>\n";

  msg += "ey: ";
  msg += dtostrf(euy, 3, 3, buf);
  msg += "<br>\n";

  msg += "ez: ";
  msg += dtostrf(euz, 3, 3, buf);
  msg += "<br>\n";

  msg += "kp: ";
  msg += dtostrf(PID_P, 3, 3, buf);
  msg += "<a href=\"/p-\"> p- </a><a href=\"/p+\"> p+ </a><br>\n";

  msg += "kd: ";
  msg += dtostrf(PID_D, 3, 3, buf);
  msg += "<a href=\"/d-\"> d- </a><a href=\"/d+\"> d+ </a><br>\n";

  msg += "</html>";
  server.send(200, "text/html", msg);
  //digitalWrite(LED_BUILTIN, 0);
}
void handleNotFound() {
  //digitalWrite(LED_BUILTIN, 1);
  String msg = "File Not Found\n\n";
  msg += "URI: ";
  msg += server.uri();
  msg += "\nMethod: ";
  msg += (server.method() == HTTP_GET) ? "GET" : "POST";
  msg += "\nArguments: ";
  msg += server.args();
  msg += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    msg += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", msg);
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

  server.on("/p-", []() {
    PID_P -= 0.1;
    if(PID_P < 0)PID_P = 0.0;
    handleRoot();
  });

  server.on("/p+", []() {
    PID_P += 0.1;
    handleRoot();
  });

  server.on("/p/{}", []() {
    String str = server.pathArg(0);
    PID_P = str.toFloat();
    handleRoot();
  });

  server.on("/d-", []() {
    PID_D -= 0.1;
    if(PID_D < 0)PID_D = 0.0;
    handleRoot();
  });

  server.on("/d+", []() {
    PID_D += 0.1;
    handleRoot();
  });

  server.on("/d/{}", []() {
    String str = server.pathArg(0);
    PID_D = str.toFloat();
    handleRoot();
  });

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
  q0 = 1.0;
  q1 = q2 = q3 = 0.0;
  integralx = integraly = integralz = 0.0;
}
void init_pwm()
{
  pinMode(EN_L, OUTPUT);
  pinMode(EN_R, OUTPUT);

  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);

  ledcAttachPin(PWM_L, CH0);
  //ledcSetup(CH0, 2, 16);
  //ledcWrite(CH0, 32768);

  ledcAttachPin(PWM_R, CH1);
  //ledcSetup(CH1, 2, 16);
  //ledcWrite(CH1, 32768);
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

  init_pwm();
}
