#pragma once

#include "CScript.h"

#include "GameObject.h"

class CRelicScript : public CScript {
public:
    CRelicScript();
    virtual ~CRelicScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    CLONE(CRelicScript);

public:
    void SetTarget(Ptr<GameObject> target) { mTarget = target; }
    void SetAlbedo(const Vec4& color);

private:
    float mElapsedTime{};

    Ptr<GameObject> mTarget{};
};

