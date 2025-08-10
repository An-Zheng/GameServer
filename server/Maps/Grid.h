#ifndef GRID_H
#define GRID_H
#include "../Utilities/Util.h"
#include "Cell.h"
#include <cstdint>

template
<
uint32_t N
>
class Grid
{
private:
    uint32_t i_gridId;
    int32_t i_x;
    int32_t i_y;
    Cell i_cells[N][N];
};
#endif