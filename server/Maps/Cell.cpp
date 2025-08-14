#include "Cell.h"
#include <cstddef>

bool Cell::add(WorldObject* obj)
{
    for(auto object : m_objects)
    {
        if (obj == object) 
        {
            return false;
        }
    }

    m_objects.push_back(obj);
}

bool Cell::remove(WorldObject* obj)
{
    for(size_t i = 0; i < m_objects.size(); i++)
    {
        if(obj == m_objects[i])
        {
            m_objects[i] = m_objects.back();
            m_objects.pop_back();
            return true;
        }
    }
    return false;
}


