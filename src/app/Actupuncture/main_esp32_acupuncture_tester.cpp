//https://www.electronicshub.org/wp-content/uploads/2021/02/ESP32-Pinout-1.jpg

#include "all_devices.h"
#ifdef I_AM_ACUPUCTURE_TESTER_ESP32

#include "i2c_commu.h"
#include <string>
#include <HardwareSerial.h>


I2c_commu obj_i2c_bus = I2c_commu();

uint8_t LED_PINS[14] = {14,12,13,15, 2,4,5,18, 19,32,33,25, 26,27};

void setup() {
    Serial.begin(115200);
    obj_i2c_bus.Init();
    for (int i=0; i<14; i++){
        pinMode(LED_PINS[i], OUTPUT);
    }
}


void loop() {
    uint8_t cell_index = obj_i2c_bus.SpinOnce();
    Serial.print("cell_index=");
    Serial.print(cell_index);
    Serial.print("   ");
    
    Serial.print(obj_i2c_bus.Cells[cell_index].CurrentFlags[0], BIN);
    Serial.print("   ");
    Serial.print(obj_i2c_bus.Cells[cell_index].CurrentFlags[1], BIN);
    Serial.print("   ");
    Serial.print(obj_i2c_bus.Cells[cell_index].CurrentFlags[2], BIN);
    Serial.print("   ");
    Serial.print(obj_i2c_bus.Cells[cell_index].CurrentFlags[3], BIN);
    Serial.println(" ");
    for(int i=0; i<14; i++){
        uint8_t byte_index = 0;
        uint8_t bit_index = i;
        if (i>=8) {
            byte_index = 1;
            bit_index = i-8;
        }
        bool flag = (obj_i2c_bus.Cells[cell_index].CurrentFlags[byte_index] >> bit_index) & 0x01;
        digitalWrite(LED_PINS[i], flag);
    }
}
#endif