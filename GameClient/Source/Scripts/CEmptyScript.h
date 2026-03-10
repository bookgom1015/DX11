#pragma once

#include "CScript.h"

#include "GameObject.h"

class CEmptyScript : public CScript {
public:
    CEmptyScript();
    virtual ~CEmptyScript();

public:
    virtual void Tick() override;

public:
    CLONE(CEmptyScript);

    virtual void SaveToLevelFile(FILE* const _FileStream) override {}
    virtual void LoadFromLevelFile(FILE* const _FileStream) override {}

private:
    Ptr<GameObject> m_Object;
    float m_Float;
    Vec2 m_Vec2;
    Vec3 m_Vec3;
};