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
    if (!cell->IsOnline) return;

    Serial.print("Addr=");
    Serial.print(cell->Address);
    Serial.print("  ");
    // uint8_t slave_addr = cell->Address;
    uint8_t n_bytes = 4;
    // cell->Address = 3;
    Wire.beginTransmission(cell->Address);
    Wire.endTransmission(false);
    Wire.requestFrom(cell->Address, n_bytes);    // request data from slave device
    int i=0;
    while (Wire.available() > 0) {  // slave may send less than requested
        uint8_t c = Wire.read();         // receive a byte as character
        cell->CurrentFlags[i] = c;
        i++;
        Serial.print(".");
        // Serial.print(c, BIN);
        // Serial.print("   ");
    }
    if(i==0) {
        cell->IsOnline = false;
        Serial.println("  No response");
    } 
    Wire.endTransmission(true);
    // delay(1000);
}
uint8_t I2c_commu::SpinOnce(){
    uint8_t last_slave_index = 0;
    for (uint8_t i= 0; i< CELLS; i++){
        TouchCell* pCell = &this->Cells[i];
        if (pCell->IsOnline){
            // Serial.print("Trying I2C addr=");
            // Serial.print(pCell->Address);
            // Serial.println("   ");
            pCell->CopyCurrentToLast();
            ReadSingleCell(pCell);
            pCell->CompareCurrentAndLast();
            last_slave_index =  i;
        }
    }
    if (last_slave_index == 0){
        //All cells is offline. re init all cells to online
        for(int i=0; i< CELLS; i++){
            this->Cells[i].IsOnline = true;
        }
    }
    return last_slave_index;
}
