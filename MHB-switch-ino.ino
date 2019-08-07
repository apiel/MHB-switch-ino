#include <ESP8266mDNS.h>

#define DEVICE_NAME "MHB test"
#define DEVICE_ID "MHB_switch"
#define WEMOS // for test

void setup(void){
  Serial.begin(115200);
  wifiConnect();

  if (MDNS.begin(DEVICE_NAME)) {
    Serial.println("MDNS responder started");
  }

  relayInit();
  upnpInit();
  httpdInit();
}

void loop(void){
  httpdHandle();
  upnpHandle();
  wifiCheck();
}
