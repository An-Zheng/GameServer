#ifndef ENTITIES_H
#define ENTITIES_H
#include "../Utilities/Util.h"
#include "../Maps/Map.h"
class ObjectGuid
{
public:
    ObjectGuid() = default;
    explicit ObjectGuid(uint64_t guid) : _guid(guid) {}
    uint64_t   GetUID() const { return _guid; }
    bool operator == (ObjectGuid const & r ) const { return _guid == r._guid; }
    bool operator != (ObjectGuid const & r ) const { return _guid != r._guid; } 
private:
    uint64_t _guid;
};
enum TypeID : uint16_t
{
    TYPEID_OBJECT      = 0,
    TYPEID_ITEM        = 1,
    TYPEID_PLAYER      = 2,
    TYPEID_CORPSE      = 3
};

class Object
{
public:
    Object() = default;
    virtual ~Object() = default;
    bool IsInWorld() const { return _inWorld; }
    virtual void AddToWorld()      { _inWorld = true;  }
    virtual void RemoveFromWorld() { _inWorld = false; }

    ObjectGuid GetGUID() const { return _guid; }
    void SetGUID(ObjectGuid const &g) { _guid = g; }
private:
    bool      _inWorld = false;
    ObjectGuid _guid{};
};

struct WorldLocation
{
    uint32_t mapId = 0;
    float x = 0.f, y = 0.f, z = 0.f, o = 0.f; // orientation

    void Relocate(float nx, float ny, float nz, float no)
    {
        x = nx; y = ny; z = nz; o = no;
    }
};

class WorldObject : public Object, public WorldLocation
{

public:
    explicit WorldObject(std::string name ) : _name(name){};
    ~WorldObject() override = default;
    // 地图绑定
    virtual void SetMap(Map* m) { _map = m; }
    Map* GetMap() const { return _map; }
    
private:
    Map*        _map = nullptr;
    std::string _name;
};
#endif