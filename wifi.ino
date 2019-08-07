#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MyWifiSettings.h>

void wifiCheck(){
  if (WiFi.status() != WL_CONNECTED) { // this might not be enough
    Serial.println("\nDisconnected from Wifi, reset.");
    ESP.reset();
  }
}

void wifiConnect(){
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

