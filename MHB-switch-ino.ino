#include <ESP8266mDNS.h>

#define FIRMWARE_VERSION "0.0.5"
#define DEVICE_NAME "wc light"
#define DEVICE_ID "MHB_switch"

// #define WEMOS // for test
#define RELAY_TIMER
// for sonoff use generic esp8266 - 1M no Spiffs

void setup(void){
  Serial.begin(115200);
  Serial.print("Firmware version ");
  Serial.println(FIRMWARE_VERSION);
  wifiConnect();

  if (MDNS.begin(DEVICE_NAME)) {
    Serial.println("MDNS responder started");
  }

  relayInit();
  httpdInit();
  buttonInit();
}

void loop(void){
  httpdHandle();
  upnpHandle();
  wifiCheck();
  buttonHandle();
  relayHandleTimer();
}
