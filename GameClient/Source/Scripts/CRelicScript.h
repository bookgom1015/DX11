#pragma once

#include "CScript.h"

#include "GameObject.h"

class CRelicScript : public CScript {
public:
    CRelicScript();
    virtual ~CRelicScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    CLONE(CRelicScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

public:
    void SetTarget(Ptr<GameObject> target) { mTarget = target; }
    void SetAlbedo(const Vec4& color);

private:
    float mElapsedTime{};

    Ptr<GameObject> mTarget{};
};

