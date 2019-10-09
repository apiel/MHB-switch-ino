#ifdef USE_EEPROM

#include <EEPROM.h>

#define EEPROM_SIZE 50

void eepromBegin() {
  EEPROM.begin(EEPROM_SIZE);
}

String eepromRead() {
  char value[EEPROM_SIZE];
  unsigned int len=0;
  unsigned char k;
  k=EEPROM.read(0);
  while(k != '\0' && len < EEPROM_SIZE - 1) {
    k=EEPROM.read(len);
    value[len]=k;
    len++;
  }
  value[len]='\0';
  Serial.println("Read eeprom (" + String(len) + "): '" + String(value) + "'");
  return String(value);
}

void eepromWrite(String value) {
  unsigned int len = value.length();
  for(unsigned int i=0; i < len && i < EEPROM_SIZE - 1;i++) {
    EEPROM.write(i, value[i]);
  }
  EEPROM.write(len,'\0');
  EEPROM.commit();
  Serial.println("Write eeprom (" + String(len) + "): '" + value + "'");
}

#endif

