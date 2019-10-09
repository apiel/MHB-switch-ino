#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MyWifiSettings.h>

void wifiDisconnected() {
    Serial.println("\nDisconnected from Wifi, reset.");
    // ESP.reset();
    wifiReConnect();
}

#define MAX_RETRY_WIFI_CHECK 3
unsigned int retryWifiCheck = 0;
unsigned long lastWifiCheck = 0;
const unsigned long intervalWifiCheck = 10UL*1000UL; // 10sec
bool wifiCheckWithUrl() {
  if (millis() > lastWifiCheck + intervalWifiCheck) {
    HTTPClient http;
    String url = "http://192.168.0.1/";
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("Wifi check call (" + url + "): " + String(httpCode));
    http.end();
    lastWifiCheck = millis();
    if (httpCode > 0) {
      retryWifiCheck++;
    } else {
      retryWifiCheck = 0;
    }
    if (retryWifiCheck > MAX_RETRY_WIFI_CHECK) {
      retryWifiCheck = 0;
      return false;
    }
  }
  return true;
}

void wifiCheck(){
  if (WiFi.status() != WL_CONNECTED || wifiCheckWithUrl() != true) {
    wifiDisconnected();
  }
}

void wifiBegin(){
  // wifi_set_sleep_type(NONE_SLEEP_T); // for upnp
  
  WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
    Serial.println("\nOnStationModeDisconnected.");
    wifiDisconnected();
  });

  // WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
  // {
    // Serial.print("onStationModeGotIP, IP: ");
    // Serial.println(WiFi.localIP());
  // });

  wifiConnect();
}

void wifiConnect(){
  Serial.println("Connect to wifi.");

  WiFi.begin(MYWIFISSID, MYWIFIPASSWORD);
  if (!wifiWait()) {
    ESP.reset();
  }
}

void wifiReConnect(){
  Serial.println("ReConnect to wifi.");

  WiFi.reconnect();
  if (!wifiWait()) {
    wifiConnect();
  }
}

bool wifiWait(){
  // Wait for connection
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (attempt++ > 100) {
      Serial.println("\nCould not connect to Wifi, reset.");
      return false;
    }
  }
  Serial.println("");
  Serial.print("Station connected, IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

String macAddr = "";
String wifiGetMac(){
  if (macAddr.length() == 0) {
    byte mac[6];
    WiFi.macAddress(mac);
    macAddr = String(mac[5],HEX);
    macAddr += "-" + String(mac[4],HEX);
    macAddr += "-" + String(mac[3],HEX);
    macAddr += "-" + String(mac[2],HEX);
    macAddr += "-" + String(mac[1],HEX);
    macAddr += "-" + String(mac[0],HEX); 
  }
  return macAddr;
}

