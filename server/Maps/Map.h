#ifndef MAP_H
#define MAP_H
#include "../Database/DBDataStructure.h"
#include "Grid.h"
#define MAX_NUMBER_OF_GRIDS 64
#define MAX_NUMBER_OF_CELLS 8
class Map 
{
public:
    Map(MapEntry map);

protected:

    Grid<MAX_NUMBER_OF_GRIDS> i_grids;
    uint32_t i_InstanceId;
    MapEntry i_mapEntry;
};
#endif