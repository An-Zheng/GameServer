#ifndef MAP_H
#define MAP_H
#include "../Database/DBDataStructure.h"
#define MAX_NUMBER_OF_GRIDS 64

class Map 
{
public:
    Map(MapEntry map);

protected:

    uint32_t i_InstanceId;
    MapEntry i_mapEntry;
};
#endif