#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MyWifiSettings.h>

void wifiDisconnected() {
    Serial.println("\nDisconnected from Wifi, reset.");
    ESP.reset();
}

void wifiCheck(){
  if (WiFi.status() != WL_CONNECTED) {
    wifiDisconnected();
  }
}

void wifiConnect(){
  WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
    wifiDisconnected();
  });
  
  // wifi_set_sleep_type(NONE_SLEEP_T); // for upnp
  WiFi.begin(MYWIFISSID, MYWIFIPASSWORD);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String wifiGetMac(){
  byte mac[6];
  WiFi.macAddress(mac);
  String str = String(mac[5],HEX);
  str += "-" + String(mac[4],HEX);
  str += "-" + String(mac[3],HEX);
  str += "-" + String(mac[2],HEX);
  str += "-" + String(mac[1],HEX);
  str += "-" + String(mac[0],HEX);
  return str;
}

