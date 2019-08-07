#ifdef WEMOS
    #define PIN_RELAY 5 // D1
#else
    #define PIN_RELAY 12
#endif

#define RELAY_ON 1
#define RELAY_OFF 0

int relayState = RELAY_OFF;

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

int relayGetState() {
  return relayState;
}


