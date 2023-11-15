#include <global_variable.h>
void writeString(uint16_t address, String data) {
  uint16_t len = data.length();
  EEPROM.write((address - 100) / 100, len);
  EEPROM.commit();
  delayMicroseconds(10);
  for (uint16_t i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
    EEPROM.commit();
    delayMicroseconds(10);
  }
}

String readString(uint16_t address) {
  uint16_t len = EEPROM.read((address - 100) / 100);
  String data = "";
  delayMicroseconds(10);
  for (uint16_t i = 0; i < len; i++) {
    data += (char)EEPROM.read(address + i);
    delayMicroseconds(10);
  }
  return data;
}