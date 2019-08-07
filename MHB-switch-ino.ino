#include <ESP8266mDNS.h>

#define DEVICE_NAME "ESP_test"
#define WEMOS // for test

void setup(void){
  Serial.begin(115200);
  wifiConnect();

  if (MDNS.begin(DEVICE_NAME)) {
    Serial.println("MDNS responder started");
  }

  relayInit();
  httpdInit();
}

void loop(void){
  httpdHandle();
  wifiCheck();
}
