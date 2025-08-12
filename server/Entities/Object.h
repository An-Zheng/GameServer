#ifndef ENTITIES_H
#define ENTITIES_H

#include "../Utilities/Util.h"
#include "../Maps/Map.h"

// class ObjectGuid
// {
// public:
//     ObjectGuid() = default;
//     explicit ObjectGuid(uint64_t guid) : _guid(guid) {}
//     uint64_t   GetUID() const { return _guid; }
//     bool operator == (ObjectGuid const & r ) const { return _guid == r._guid; }
//     bool operator != (ObjectGuid const & r ) const { return _guid != r._guid; } 
// private:
//     uint64_t _guid;
// };
enum TypeID : uint16_t
{
    TYPEID_OBJECT      = 0,
    TYPEID_ITEM        = 1,
    TYPEID_FIGHT_UNIT  = 2,
    TYPEID_PLAYER      = 3,
    TYPEID_CORPSE      = 4
};

class Object
{
public:
    explicit Object();
    virtual ~Object() = default;
    bool IsInWorld() const { return m_inWorld; }
    virtual void AddToWorld()      { m_inWorld = true;  }
    virtual void RemoveFromWorld() { m_inWorld = false; }

    uint64_t GetGUID() const { return m_guid; }
    void SetGUID(uint64_t const &g) { m_guid = g; }

    TypeID GetTypeId() const { return m_typeId; }
protected:
    TypeID   m_typeId;
private:
    bool     m_inWorld;
    uint64_t m_guid;
    static std::atomic<uint64_t> _counter;
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

protected:
    explicit WorldObject(std::string name ) ;
public:
    virtual ~WorldObject() override = default;

    void AddToWorld() override;
    void RemoveFromWorld() override;
    // 地图绑定
    virtual void SetMap(Map* m) { m_map = m; }
    Map* GetMap() const { return m_map; }
    float GetDistance( WorldObject const *other);
    // 可见性/广播空壳
    virtual void UpdateObjectVisibility(bool /*forced*/ = true) {}
private:
    Map*        m_map;
    std::string m_name;
};

class FightUnit : public WorldObject
{
protected:
    FightUnit(std::string name);

    // Update（tick）—— 驱动移动/光环/施法等（此处极简）
    virtual void Update(uint32_t /*diffMs*/) {}

    void SetMaxHealth(uint32_t v) { m_maxHealth = v; if (m_health > m_maxHealth) m_health = m_maxHealth; }
    void SetHealth(uint32_t v)    { m_health = (v > m_maxHealth) ? m_maxHealth : v; }
    uint32_t GetMaxHealth() const { return m_maxHealth; }
    uint32_t GetHealth() const    { return m_health; }
    bool IsAlive() const          { return m_health > 0; }

    void SetMaxPower(uint32_t v)  { m_maxPower = v; if (m_power > m_maxPower) m_power = m_maxPower; }
    void SetPower(uint32_t v)     { m_power = (v > m_maxPower) ? m_maxPower : v; }
    uint32_t GetMaxPower() const  { return m_maxPower; }
    uint32_t GetPower() const     { return m_power; }
public:
    virtual ~FightUnit() override = default;
    
    uint32_t m_maxHealth;
    uint32_t m_health;
    uint32_t m_maxPower;
    uint32_t m_power;
};


class Player : public FightUnit
{
public:
    Player(std::string name);

    // 账号/角色标识（示例）
    void SetAccountId(uint32_t id) { _accountId = id; }
    uint32_t GetAccountId() const  { return _accountId; }


    // 传送（极简：仅改位置/地图，不做可见性/载具/副本校验）
    bool TeleportTo(uint32_t newMapId, float nx, float ny, float nz, float no)
    {
        // TODO: 执行存盘/卸载周边/可见性重建等
        mapId = newMapId;
        Relocate(nx, ny, nz, no);
        return true;
    }

    // 保存（空壳）
    void SaveToDB() const
    {
        // TODO: 把 guid/位置/资源/背包 等写入数据库
    }

    // 玩家更新（可覆盖）
    void Update(uint32_t diffMs) override
    {
        FightUnit::Update(diffMs);
        // TODO: 驱动玩家状态/移动输入/冷却/脚本等
    }

private:
    uint32_t _accountId = 0;
};
#endif