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

public:
    CLONE(CTargetMissileScript);

public:
    void BeginOverlap(CollisionData pOwner, CollisionData pOther);
    void Overlap(CollisionData pOwner, CollisionData pOther);
    void EndOverlap(CollisionData pOwner, CollisionData pOther);

public:
    void SetTarget(Ptr<GameObject> target) { mTarget = target; }

private:
    void CreateParticles(Vec3 pos, bool blood);

private:
    Ptr<GameObject> mTarget{};

    float mElapsedTime{};
};

