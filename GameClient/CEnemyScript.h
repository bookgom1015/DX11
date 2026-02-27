#pragma once
#include "CScript.h"

#include "GameObject.h"

class CEnemyScript : public CScript {
public:
    CEnemyScript();
    virtual ~CEnemyScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
};

