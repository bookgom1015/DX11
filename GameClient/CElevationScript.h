#pragma once
#include "CScript.h"

#include "GameObject.h"

class CElevationScript : public CScript {
public:
    CElevationScript();
    virtual ~CElevationScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    void SetStopOverPos(const Vec3& pos) { mStopOverPos = pos; }

    void SetSpeed(float speed) { mSpeed = speed; }

private:
    float mElapsedTime{};

    Vec3 mOriginalPos{};
    Vec3 mStopOverPos{};

    float mSpeed{ 1.f };
};

