#pragma once
#include "CScript.h"
class CCamMoveScript :
    public CScript
{
private:

public:
    virtual void Tick() override;

public:
    CCamMoveScript();
    virtual ~CCamMoveScript();
};

