#pragma once

#include "Component.h"

#include "Source/ScriptMgr.h"

class CCollider2D;

struct Property {
    string Name;
    size_t Offset;

    enum EType { 
        Bool,
        Int, 
        Float, 
        Vec2, 
        Vec3,
        Object
    } Type;
};

class CScript : public Component {
public:
    CScript(int _ScriptType);
    virtual ~CScript();

public:
    virtual void TickOnce() { m_FirstTick = false; }
    virtual void Tick() = 0;
    virtual void FinalTick() final {}
    virtual void CleanUp() {}

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

public:
    GET(bool, FirstTick);

    int GetScriptType() { return m_ScriptType; }

    void GetProperties(vector<Property>& properties) { 
        for (const auto& property : m_Properties)
            properties.push_back(property);
    }

protected:
    void Destroy();

    void AddProperty(const string& name, size_t offset, Property::EType type) {
        m_Properties.emplace_back(name, offset, type);
    }


private:
    bool m_FirstTick;

    int m_ScriptType;

    vector<Property> m_Properties;
};

#define ADD_DYNAMIC_BEGIN_OVERLAP(MemFunc) Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&MemFunc);
#define ADD_DYNAMIC_OVERLAP(MemFunc) Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&MemFunc);
#define ADD_DYNAMIC_END_OVERLAP(MemFunc) Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&MemFunc);

#define ADD_PROPERTY(__script, __name, __prop, __type) AddProperty(__name, offsetof(__script, __prop), __type);