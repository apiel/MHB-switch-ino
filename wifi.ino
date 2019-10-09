#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MyWifiSettings.h>

void wifiDisconnected() {
    Serial.println("\nDisconnected from Wifi, try to reconnect...");
    // ESP.reset();
    wifiReConnect();
}

#define MAX_RETRY_WIFI_CHECK 3
unsigned int retryWifiCheck = 0;
unsigned long lastWifiCheck = 0;
const unsigned long intervalWifiCheck = 60UL*1000UL; // 1min
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
      retryWifiCheck = 0;
    } else {
      retryWifiCheck++;
    }
    if (retryWifiCheck > MAX_RETRY_WIFI_CHECK) {
      Serial.println("Wifi check with url reached max retry, trigger disconnected.");
      retryWifiCheck = 0;
      return false;
    }
  }
  return true;
}

void wifiCheck(){
  if (!WiFi.isConnected() || !WiFi.localIP().isSet() || wifiCheckWithUrl() != true) {
    Serial.println("Wifi status disconnected (" + String(WiFi.status()) + ")");
    // Serial.println(WiFi.localIP());
    wifiDisconnected();
  }
}

void wifiBegin(){
  // wifi_set_sleep_type(NONE_SLEEP_T); // for upnp
  
  WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
    Serial.println("\nOnStationModeDisconnected.");
    wifiDisconnected();
  });

  Serial.println("Auto reconnect: " + String(WiFi.getAutoReconnect()));

  wifiConnect();
}

#define MAX_WIFI_CONNECT_RETRY 3
unsigned int wifiConnectRetry = 0;
void wifiConnect(){
  Serial.println("Connect to wifi.");

  WiFi.disconnect();
  // WiFi.mode(WIFI_OFF);
  // WiFi.mode(WIFI_STA);
  WiFi.begin(MYWIFISSID, MYWIFIPASSWORD);
  if (!wifiWait()) {
    if (wifiConnectRetry < MAX_WIFI_CONNECT_RETRY) {
      wifiConnectRetry++;
      wifiConnect();
    } else {
      ESP.reset(); 
    }
  } else {
    wifiConnectRetry = 0;
  }
}

void wifiReConnect(){
  Serial.println("ReConnect to wifi.");

  WiFi.reconnect();
  if (!wifiWait()) {
    wifiConnect();
  }
}

// could use WiFi.waitForConnectResult() instead
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

