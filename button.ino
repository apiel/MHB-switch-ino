#include <ClickButton.h>

#define PIN_BUTTON 0 // D3
#define PIN_BUTTON2 14 // D5

ClickButton btn1(PIN_BUTTON, LOW, CLICKBTN_PULLUP);

#ifdef BTN2_ENABLE
ClickButton btn2(PIN_BUTTON2, LOW, CLICKBTN_PULLUP);
#endif

void buttonInit() {
  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  btn1.debounceTime   = 20;   // Debounce timer in ms
  btn1.multiclickTime = 250;  // Time limit for multi clicks
  btn1.longClickTime  = 3000; // time until "held-down clicks" register

  #ifdef BTN2_ENABLE
  btn2.debounceTime   = 20;   // Debounce timer in ms
  btn2.multiclickTime = 250;  // Time limit for multi clicks
  btn2.longClickTime  = 3000; // time until "held-down clicks" register
  #endif
}

void buttonDefaultActions(ClickButton * btn) {
  if (btn->clicks == 1) { // single click
    Serial.println("Button click.");
    #ifdef BTN_RELAY_TIMER
      relayToggleTimer();
    #else
      relayToggle();
    #endif
  } else if (btn->clicks > 5) { // more than 5 click
    Serial.println("Button mutliple click.");
    Serial.println("Reboot...");
    ESP.reset();
  } else if (btn->clicks < 0) { // long press over 3 second
    Serial.println("Button long click.");
  }
}

void buttonHandle() {
  btn1.Update();
  buttonDefaultActions(&btn1);

  #ifdef BTN2_ENABLE
  btn2.Update();
    #ifdef BTN2_MIRROR_DEFAULT
    buttonDefaultActions(&btn2);
    #endif

  if (btn1.clicks != 0 || btn2.clicks != 0) {
  #else
  if (btn1.clicks != 0) {
  #endif
    String params = "&btn1=" + String(btn1.clicks);
    #ifdef BTN2_ENABLE
    params += "&btn2=" + String(btn2.clicks);
    #endif
    callMiddleware("button", params);
  }
}
