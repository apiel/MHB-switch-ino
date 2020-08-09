#include <ESP8266mDNS.h>
#include <ClickButton.h> // need to be there for buttonDefaultActions
#include <ESPAsyncWebServer.h> // need to be there for httpd

#define FIRMWARE_VERSION "0.0.22"
#define DEVICE_ID "MHB_switch"

// #define WEMOS // for test
// #define BTN_RELAY_TIMER
// #define RF_ENABLE
#define BTN2_ENABLE
#define BTN2_MIRROR_DEFAULT
#define USE_EEPROM

// for sonoff basic use generic esp8266 - 1M no Spiffs - DOUT

// ToDo
// - fix ota with server async -> HTTP_UPDATE_FAILD Error (-1): HTTP error: connection refused

void setup(void){
  Serial.begin(115200);
  Serial.print("Firmware version ");
  Serial.println(FIRMWARE_VERSION);
  wifiBegin();
  #ifdef USE_EEPROM
    eepromBegin();
  #endif

  if (MDNS.begin(String(DEVICE_ID))) {
    Serial.println("MDNS responder started");
  }

  relayInit();
  httpdInit();
  buttonInit();
  #ifdef RF_ENABLE
    rfInit();
  #endif
  SSDPbegin();
  thingInit();
}

void loop(void){
  SSDPhandle();
  wifiCheck();
  buttonHandle();
  relayHandleTimer();
  arpHandle();
  thingHandle();
}

