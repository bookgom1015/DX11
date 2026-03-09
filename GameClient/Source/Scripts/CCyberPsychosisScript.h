#pragma once

#include "CScript.h"

#include "GameObject.h"

class CCyberPsychosisScript : public CScript {
public:
    CCyberPsychosisScript();
    virtual ~CCyberPsychosisScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:
    CLONE(CCyberPsychosisScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

public:
    void SetOffset(const Vec3& offset) { mOffset = offset; }
    void SetTarget(Ptr<GameObject> target) { mTarget = target; }

private:
    float mElapsedTime{};
    float mLifeTime{};

    Vec4 mColor{};

    Vec3 mOffset{};
    Ptr<GameObject> mTarget{};
};

