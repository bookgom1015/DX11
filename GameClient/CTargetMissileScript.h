#pragma once

#include "CScript.h"
#include "GameObject.h"

class CTargetMissileScript : public CScript {
public:
    CTargetMissileScript();
    virtual ~CTargetMissileScript();

public:
    virtual void Begin();
    virtual void Tick();

    void BeginOverlap(CCollider2D* pOwner, CCollider2D* pOther);
    void Overlap(CCollider2D* pOwner, CCollider2D* pOther);
    void EndOverlap(CCollider2D* pOwner, CCollider2D* pOther);

public:
    void SetTarget(Ptr<GameObject> target) { mTarget = target; }

private:
    void SelfDestruct();

private:
    Ptr<GameObject> mTarget{};

    float mElapsedTime{};
};

