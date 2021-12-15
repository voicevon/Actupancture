#pragma once
#include "stdint.h"


class TouchCell{
    public:
        uint8_t Address;
        bool IsOnline = true;
        uint8_t CurrentFlags[4];
        uint8_t LastFlags[4];
        bool HasUpdate();
        bool CompareCurrentAndLast();
        void CopyCurrentToLast();
        const char* GetName(int point_id);
        bool IsBitUpdated(int bit_index);
        const char* GetMqttPayload(int bit_index);

    private:
        bool _has_update;


};