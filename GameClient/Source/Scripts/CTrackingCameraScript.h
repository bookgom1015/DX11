#pragma once

#include "CScript.h"

class CTrackingCameraScript : public CScript {
public:
    CTrackingCameraScript();
    virtual ~CTrackingCameraScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    CLONE(CTrackingCameraScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

private:
    Ptr<GameObject> mTarget{};
};

