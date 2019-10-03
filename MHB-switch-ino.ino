#include <ESP8266mDNS.h>

#define FIRMWARE_VERSION "0.0.10"
#define DEVICE_NAME "wemos light"
#define DEVICE_ID "MHB_switch"

#define WEMOS // for test
// #define BTN_RELAY_TIMER
// #define RF_ENABLE
#define BTN2_ENABLE

// for sonoff basic use generic esp8266 - 1M no Spiffs - DOUT

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
  #ifdef RF_ENABLE
    rfInit();
  #endif
  SSDPbegin();
}

void loop(void){
  httpdHandle();
  SSDPhandle();
  wifiCheck();
  buttonHandle();
  relayHandleTimer();
  arpHandle();
}

