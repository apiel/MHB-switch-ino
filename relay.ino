#ifdef WEMOS
    #define PIN_RELAY 5 // D1
#else
    #define PIN_RELAY 12
#endif

#define RELAY_ON 1
#define RELAY_OFF 0

int relayState = RELAY_OFF;

unsigned long relayTimeout = 0;
const unsigned long relayDuration = 3UL*60UL*1000UL; // 3min

void relayInit() {
  pinMode(PIN_RELAY, OUTPUT);
}

void relayOn() {
  Serial.println("Set relay ON.");
  relayState = RELAY_ON;
  digitalWrite(PIN_RELAY, relayState);
}

void relayOff() {
  Serial.println("Set relay OFF.");
  relayState = RELAY_OFF;
  digitalWrite(PIN_RELAY, relayState);
}

void relayToggle() {
  Serial.println("Relay toggle.");
  if (relayState == RELAY_OFF) {
    relayOn();
  } else {
    relayOff();
  }
}

String relayGetStatus() {
  if (relayState == RELAY_OFF) {
    return "OFF";
  }
  return "ON";
}

bool relayIsOn() {
  relayState == RELAY_ON;
}

int relayGetState() {
  return relayState;
}

void relaySetTimer() {
  relayOn();
  Serial.println("Relay set timer.");
  relayTimeout = millis() + relayDuration;
}

void relayEndTimer() {
  Serial.println("Relay end timer.");
  relayOff();
  relayTimeout = 0;
}

void relayToggleTimer() {
  if (relayTimeout > 0) {
    relayEndTimer();
  } else {
    relaySetTimer();
  }
}

void relayHandleTimer() {
  if (relayTimeout > 0 && millis() > relayTimeout) {
    relayEndTimer();
  }
}

