#include "touch_cell.h"
#include "HardwareSerial.h"
bool TouchCell::HasUpdate(){
    return this->_has_update;
}

const char* TouchCell::GetName(int point_id){
    switch (this->Address){
        case 2:  // right foot
            switch (point_id)
            {
            case 0:
                return "foot/yongquan";
                break;
            case 4:
                return "foot/qita";
                break;
            default:
                break;
            }

    }

}


bool TouchCell::CompareCurrentAndLast(){
    for(int i=0; i<4; i++){
        if (LastFlags[i] != CurrentFlags[i]){
            Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
            this->_has_update = true;
            return true;   
        }
    }
    this->_has_update = false;
    return false;
}

void TouchCell::CopyCurrentToLast(){
    for(int i =0; i<4; i++){
        this->LastFlags[i] = this->CurrentFlags[i];
    }
}
bool TouchCell::IsBitUpdated(int bit_index){
    int byte_index = 0;
    if (bit_index>=8){
        byte_index++;
        bit_index -=8;
    }
    if ((CurrentFlags[byte_index] ^ LastFlags[byte_index]) & (1<<bit_index))
        return true;
    return false;
}
const char* TouchCell::GetMqttPayload(int bit_index){
    int byte_index = 0;
    if (bit_index>=8){
        byte_index++;
        bit_index -=8;
    }
    if (CurrentFlags[byte_index] & (1<<bit_index))
        return "OFF";
    return "ON";
}