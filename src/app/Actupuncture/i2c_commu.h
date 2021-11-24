#pragma once

#include <Wire.h>
#include "touch_cell.h"

#define CELLS 2
#define START_CELL_ID 3




class I2c_commu{
    public:
        void Init();
        void SpinOnce();
        void ReadSingleCell(TouchCell* cell);
        bool HasUpdate();
        TouchCell Cells[CELLS];

    private:
        bool _has_update = true;
};