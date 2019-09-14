#ifdef BTN2_ENABLE

#include <ClickButton.h>

#define PIN_BUTTON2 14 // D5

// ClickButton btn2(PIN_BUTTON2);
ClickButton btn2(PIN_BUTTON2, HIGH, CLICKBTN_PULLUP);

void button2Init() {
  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  btn2.debounceTime   = 20;   // Debounce timer in ms
  btn2.multiclickTime = 250;  // Time limit for multi clicks
  btn2.longClickTime  = 3000; // time until "held-down clicks" register
}

void button2Handle() {
  btn2.Update();

  if (btn2.clicks == 1) { // single click
    Serial.println("Button2 click.");
  } else if (btn2.clicks > 4) { // more than 4 click
    Serial.println("Button2 mutliple click.");
  } else if (btn2.clicks < 0) { // long press over 3 second
    Serial.println("Button2 long click.");
  }

  if (btn2.clicks != 0) {
    String params = "&btn=2&clicks=" + String(btn2.clicks);
    callMiddleware("button", params);
  }
}

#endif
