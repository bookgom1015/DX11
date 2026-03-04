#pragma once

#include "CScript.h"

class CCamMoveScript : public CScript {
public:
    CCamMoveScript();
    virtual ~CCamMoveScript();

public:
    virtual void Tick() override;

public:
    CLONE(CCamMoveScript);

private:
    void MovePerspective();
    void MoveOrthographic();
};

