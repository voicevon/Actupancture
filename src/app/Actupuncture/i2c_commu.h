#pragma once



#include <Wire.h>
#include "touch_cell.h"

#define CELLS 20
#define START_CELL_ID 2


//  Pull-up resistor = 1.8K   to 5V
//
//

class I2c_commu{
    public:
        void Init();
        uint8_t SpinOnce();   // return the last slave_id of avaliable.
        void ReadSingleCell(TouchCell* cell);
        bool HasUpdate();
        TouchCell Cells[CELLS];

    private:
        bool _has_update = true;
};