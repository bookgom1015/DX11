#pragma once
#include "CScript.h"

#include "GameObject.h"

class CPlatfomerPlayerScript : public CScript {
public:
    CPlatfomerPlayerScript();
    virtual ~CPlatfomerPlayerScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;
};

