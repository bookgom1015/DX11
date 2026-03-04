#pragma once

#include "Component.h"

class CCollider2D;

class CScript : public Component {
public:
    CScript();
    virtual ~CScript();

public:
    virtual void Tick() = 0;
    virtual void FinalTick() final {}

protected:
    void Destroy();
};

#define ADD_DYNAMIC_BEGIN_OVERLAP(MemFunc) Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&MemFunc);
#define ADD_DYNAMIC_OVERLAP(MemFunc) Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&MemFunc);
#define ADD_DYNAMIC_END_OVERLAP(MemFunc) Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&MemFunc);
