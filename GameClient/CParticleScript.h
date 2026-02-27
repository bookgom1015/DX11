#pragma once
#include "CScript.h"

#include "GameObject.h"

class CParticleScript : public CScript {
public:
    CParticleScript();
    virtual ~CParticleScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

    void BeginOverlap(CollisionData pOwner, CollisionData pOther);
    void Overlap(CollisionData pOwner, CollisionData pOther);
    void EndOverlap(CollisionData pOwner, CollisionData pOther);

public:
    void SetFromColor(const Vec3& color) { mFromColor = color; }
    void SetToColor(const Vec3& color) { mToColor = color; }

private:
    void SelfDestruct();

private:
    float mElapsedTime{};

    Vec3 mFromColor{};
    Vec3 mToColor{};
};

