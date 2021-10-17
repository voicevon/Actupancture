#pragma once
#include "stdint.h"


class TouchCell{
    public:
        uint8_t Address;
        uint8_t CurrentFlags[4];
        uint8_t LastFlags[4];
        bool HasUpdate();
        bool CompareCurrentAndLast();
        void CopyCurrentToLast();

    private:
        bool _has_update;


};