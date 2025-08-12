#include "Object.h"
Object::Object()
{
    m_inWorld = false;
    m_guid = ++_counter;
    m_typeId = TYPEID_OBJECT;
}
void WorldObject::AddToWorld() 
{
    Object::AddToWorld();
    // TODO: 挂接网格/可见性/实例脚本
}
void WorldObject::RemoveFromWorld()
{
    // TODO: 脱离网格/可见性/实例脚本
    Object::RemoveFromWorld();
}
WorldObject::WorldObject(std::string name ) : Object() ,m_name(name){
    m_map = nullptr;
};

float WorldObject::GetDistance( WorldObject const *other)
{
    if (!other) return 0.f;
    float dx = x - other->x, dy = y - other->y, dz = z - other->z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
};

FightUnit::FightUnit(std::string name):WorldObject(name)
{
    m_typeId = TYPEID_FIGHT_UNIT;
    m_maxHealth = 100;
    m_health    = 100;
    m_maxPower  = 100;
    m_power     = 100;
};

Player::Player(std::string name):FightUnit(name)
{
    m_typeId = TYPEID_PLAYER;
}