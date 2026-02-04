#pragma once

#include "CScript.h"

class CTrackingCameraScript : public CScript {
public:
    CTrackingCameraScript();
    virtual ~CTrackingCameraScript();

public:
    virtual void Tick() override;

public:
    void SetTarget(Ptr<GameObject> target);

private:
    Ptr<GameObject> mTarget{};
};

