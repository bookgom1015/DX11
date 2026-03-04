#pragma once

#include "CScript.h"

#include "GameObject.h"

class CFlashLightScript : public CScript {
public:
    CFlashLightScript();
    virtual ~CFlashLightScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    CLONE(CFlashLightScript);

private:
    Ptr<GameObject> mFlashLight{};
    Ptr<GameObject> mFlashLight2{};
};

