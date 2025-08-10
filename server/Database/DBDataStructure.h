#ifndef DBDATASTRUCTURE_H
#define DBDATASTRUCTURE_H
#include "../Utilities/Util.h"

enum MapTypes                                               // Lua_IsInInstance
{
    MAP_COMMON          = 0,                                // none
    MAP_INSTANCE        = 1,                                // party
    MAP_RAID            = 2,                                // raid
    MAP_BATTLEGROUND    = 3,                                // pvp
    MAP_ARENA           = 4                                 // arena
};


struct MapEntry
{
    uint32_t ID;
    uint32_t MapType;
};
#endif 