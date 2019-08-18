#include <ClickButton.h>

#ifdef WEMOS
    #define PIN_BUTTON 0 // D3
#else
    #define PIN_BUTTON 0
#endif

ClickButton btn(PIN_BUTTON, LOW, CLICKBTN_PULLUP);

void buttonInit() {
  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  btn.debounceTime   = 20;   // Debounce timer in ms
  btn.multiclickTime = 250;  // Time limit for multi clicks
  btn.longClickTime  = 3000; // time until "held-down clicks" register
}

void buttonHandle() {
  btn.Update();

  if (btn.clicks == 1) { // single click
    Serial.println("Button click.");
  } else if (btn.clicks > 4) { // more than 4 click
    Serial.println("Button mutliple click.");
  } else if (btn.clicks < 0) { // long press over 3 second
    Serial.println("Button long click.");
  }
}

