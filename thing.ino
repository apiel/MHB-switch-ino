#define ARDUINOJSON_USE_LONG_LONG 1

#include <WebThingAdapter.h>
#include <Thing.h>

WebThingAdapter *adapter;

const char *ledTypes[] = {"OnOffSwitch", "Sonoff", nullptr};
ThingDevice led("switch", "Built-in switch", ledTypes);
ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = relayIsOn();
int portAdapter = 8080;

void thingInit() {
  Serial.println("Init mozilla iot Thing");

  adapter = new WebThingAdapter("w25", WiFi.localIP(), portAdapter);

  led.addProperty(&ledOn);
  adapter->addDevice(&led);
  adapter->begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(portAdapter);
  Serial.print("/things/");
  Serial.println(led.id);
}

void thingHandle() {
  adapter->update();
  bool on = ledOn.getValue().boolean;
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

