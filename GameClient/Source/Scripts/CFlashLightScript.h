#pragma once

#include "CScript.h"

#include "GameObject.h"

class CFlashLightScript : public CScript {
public:
    CFlashLightScript();
    virtual ~CFlashLightScript();

public:
    virtual void TickOnce() override;
    virtual void Tick() override;
    virtual void CleanUp() override;

public:
    CLONE(CFlashLightScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

private:
    Ptr<GameObject> mFlashLight{};
    Ptr<GameObject> mFlashLight2{};
};

