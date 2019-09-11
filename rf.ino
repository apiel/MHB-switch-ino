#ifdef RF_ENABLE

#include <RfReceiver.h>

#define PIN_RF 14 // D5

void rfCallback(char * result) {
  Serial.println(result);
}

void rfInit(){
  rfReceiver.enable(14, rfCallback);
}

#endif
