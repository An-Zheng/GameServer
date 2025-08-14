#ifndef CELL_H
#define CELL_H
#include "../Utilities/Util.h"
#include "../Entities/Object.h"
#include <vector>
class Cell
{
public:
    bool add(WorldObject* obj);
    bool remove(WorldObject* obj);

    const std::vector<WorldObject*> GetObjects() const { return m_objects;}
private:
    std::vector<WorldObject*> m_objects;
};
#endif