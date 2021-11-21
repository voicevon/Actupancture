#pragma once
#include "stdint.h"
#include "Arduino.h"

class HcSr04{
    public:
        HcSr04(uint8_t pin_trig, uint8_t pin_echo);
        int CheckDistance(bool show_debug);
    private:
        uint8_t __pin_trig;
        uint8_t __pin_echo;
};
