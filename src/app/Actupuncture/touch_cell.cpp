#include "touch_cell.h"
#include "HardwareSerial.h"

#define PPD2 0
#define PPD3 1
#define PPD4 2
#define PPD5 3
#define PPD6 4
#define PPD7 5
#define PPD8 6
#define PPD9 7
#define PPD10 8
#define PPD11 9

#define PPA0 10   // D14
#define PPA1 11   // D15
#define PPA2 12   // D16
#define PPA3 13   // D17




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
        case 3:
            switch (point_id)
            {
            case PPA3:  //解溪穴
                return "foot/jiexi";
                break;
            case PPD2:  //太溪穴
                return "foot/taixi";
                break;
            case PPD3:  //大钟穴
                return "foot/dazhong";
                break;
            case PPD4:  //水泉穴
                return "foot/shuiquan";
                break;
            case PPD5:  //冲阳穴
                return "foot/chongyang";
                break;
            case PPD6:  //太冲穴
                return "foot/taichong";
                break;
            case PPD7:   //然谷穴
                return "foot/rangu";
                break;
            case PPD8:   //太白穴
                return "foot/taibai";
                break;
            case PPD9:   //申脉穴
                return "foot/shenmai";
                break;
            case PPD10:   //公孙穴
                return "foot/gongsun";
                break;
            case PPD11:   //京骨穴
                return "foot/jinggu";
                break;
            case PPA0:   //金门穴
                return "foot/jinmen";
                break;
            case PPA1:  // 昆仑穴
                return "foot/kunlun";
                break;
            case PPA2:  //商丘穴
                return "foot/shangqiu";
                break;
            
            default:
                break;
            }
        case 4:
            switch (point_id)
            {
            case PPA3:  //仆参穴
                return "foot/pushen";
                break;
            case PPD6:  //
                return "foot/4_A1";
                break;
            case PPD7:  //
                return "foot/4_A2";
                break;
            case PPD11:  //
                return "foot/4_A3";
                break;
            case PPD5:  // 大都穴
                return "foot/dadu";
                break;
            case PPD3:  // 
                return "foot/4_D3";
                break;
            case PPD4:   //
                return "foot/4_D4";
                break;
            case PPD8:   // 独阴穴
                return "foot/duyin";
                break;
            case PPD9:   // 足通谷
                return "foot/zutonggu";
                break;
            case PPD10:   // 地五汇
                return "foot/diwuhui";
                break;
            case PPA1:  // 束骨穴
                return "foot/shugu";
                break;
            case PPA2:  // 涌泉穴
                return "foot/yongquan";
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
    if (bit_index >= 8){
        byte_index++;
        bit_index -= 8;
    }
    if (CurrentFlags[byte_index] & (1<<bit_index))
        return "ON";
    return "OFF";
}