#define ARDUINOJSON_USE_LONG_LONG 1

#include <WebThingAdapter.h>
#include <Thing.h>

WebThingAdapter *adapter;

const char *deviceTypes[] = {"OnOffSwitch", nullptr};
ThingDevice device("switch", "Sonoff", deviceTypes);
ThingProperty deviceOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = relayIsOn();
int portAdapter = 8080;

void thingInit() {
  Serial.println("Init mozilla iot Thing");

  adapter = new WebThingAdapter("w25", WiFi.localIP(), portAdapter);

#ifdef USE_EEPROM
  device.title = eepromRead().c_str();
#else
  String uid = String(DEVICE_ID) + "-" + WiFi.macAddress();
  device.title = uid.c_str();
#endif
  
  device.addProperty(&deviceOn);
  adapter->addDevice(&device);
  adapter->begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(portAdapter);
  Serial.print("/things/");
  Serial.println(device.id);
}

void thingHandle() {
  adapter->update();
  bool on = deviceOn.getValue().boolean;
  if (on != lastOn) {
    Serial.println("Thing action.");
    if (on) {
      relayOff();
    } else {
      relayOn();
    }
  }
  lastOn = on;
}

