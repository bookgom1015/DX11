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
    CLONE(CEnemyScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}
};

