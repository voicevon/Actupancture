//https://www.electronicshub.org/wp-content/uploads/2021/02/ESP32-Pinout-1.jpg

#include "all_devices.h"
#ifdef I_AM_ESP32

#include <Wire.h>
#include <HardwareSerial.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();     
}

void loop() {
  uint8_t slave_addr = 0x2;
  uint8_t n_bytes = 4;
  Wire.requestFrom(slave_addr, n_bytes);    // request data from slave device

  while (Wire.available() > 0) {  // slave may send less than requested
    char c = Wire.read();         // receive a byte as character
    Serial.print(c,BIN);
    Serial.print("   ");
  }
  Serial.println("");

  delay(100);
}
#endif
