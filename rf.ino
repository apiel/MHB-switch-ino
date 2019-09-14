#ifdef RF_ENABLE

// might need to re-think RfReceiver to not use ISR intterupt
// ISR intterupt might not work well with time
#include <RfReceiver.h>

#define PIN_RF 14 // D5

void rfCallback(char * result) {
  Serial.println(result);
}

void rfInit(){
  rfReceiver.enable(14, rfCallback);
}

#endif
