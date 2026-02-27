#pragma once
#include "CScript.h"

#include "GameObject.h"

class CPlayerScript :
    public CScript
{
private:
    Ptr<GameObject>      m_Target;

    float                m_Limit;


public:
    void SetTarget(Ptr<GameObject> _Target) { m_Target = _Target; }

public:
    virtual void Tick() override;

private:
    void Move();
    void Shoot();

public:
    CPlayerScript();
    virtual ~CPlayerScript();
};

