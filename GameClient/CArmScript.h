#pragma once

#include "CScript.h"

class CArmScript : public CScript {
public:
    CArmScript();
    virtual ~CArmScript();

public:
    virtual void Tick() override;
};

