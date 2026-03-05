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

private:
    Ptr<GameObject> mTarget{};
};

