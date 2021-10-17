#include "touch_cell.h"

bool TouchCell::HasUpdate(){
    return this->_has_update;
}

bool TouchCell::CompareCurrentAndLast(){
    for(int i=0; i<4; i++){
        if (LastFlags[i] != CurrentFlags[i]){
            this->_has_update = false;
            return false;   
        }
    }
    this->_has_update = true;
    return true;
}

void TouchCell::CopyCurrentToLast(){
    for(int i =0; i<4; i++){
        this->LastFlags[i] = this->CurrentFlags[i];
    }
}