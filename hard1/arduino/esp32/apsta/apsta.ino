#include <WiFi.h>
#include <WiFiAP.h>
#include "AsyncUDP.h"


//i'm accesspoint
const char* ap_ssid = "esp32";
const char* ap_pass = "88888888";
char ap_stat = 0;

//i'm station
const char* sta_ssid = "********";
const char* sta_pass = "********";
char sta_stat = 0;

//udp server
AsyncUDP udp;
int port = 1234;




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
  });
}
void setup(){
  Serial.begin(115200);

  init_wifi();
  init_udpserver();
}




void loop(){
  if(WL_CONNECTED == WiFi.status()){
    if(0 == sta_stat){
      sta_stat = 1;
      Serial.print("sta.ip: ");
      Serial.println(WiFi.localIP());
    }
  }
}
