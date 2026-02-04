#pragma once
#include "Component.h"

class CCollider2D;

class CScript :
    public Component
{
private:

public:
    virtual void Tick() = 0;
    virtual void FinalTick() final {}

public:
   //void BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) {}
   //void Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) {}
   //void EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) {}

protected:
    void Destroy();

public:
    CScript();
    virtual ~CScript();
};

