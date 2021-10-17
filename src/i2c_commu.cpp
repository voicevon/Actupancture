#include "i2c_commu.h"
#include "HardwareSerial.h"
#include <Arduino.h>
void I2c_commu::Init(){
    Wire.begin();  
    for(int i=0; i<CELLS; i++){
        Cells[i].Address = START_CELL_ID + i;
    }
} 

bool I2c_commu::HasUpdate(){
    return this->_has_update;
}

void I2c_commu::ReadSingleCell(TouchCell* cell){
    // uint8_t slave_addr = cell->Address;
    uint8_t n_bytes = 4;
    Wire.requestFrom(cell->Address, n_bytes);    // request data from slave device

    while (Wire.available() > 0) {  // slave may send less than requested
        char c = Wire.read();         // receive a byte as character
        Serial.print(c, BIN);
        Serial.print("   ");
    }
    Serial.println("");
}
void I2c_commu::SpinOnce(){
    for (uint8_t i= 0; i< CELLS; i++){
        TouchCell* pCell = &this->Cells[i];
        pCell->CopyCurrentToLast();
        ReadSingleCell(pCell);
    }

    delay(100);
}
