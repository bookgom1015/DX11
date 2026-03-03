#pragma once
#include "CScript.h"

class CMonsterScript :
    public CScript
{
private:
    float       m_Time;

public:
    virtual void Begin() override;
    virtual void Tick() override;

private:
    void BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider);
    void Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider);
    void EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider);

public:
    CMonsterScript();
    virtual ~CMonsterScript();
};

