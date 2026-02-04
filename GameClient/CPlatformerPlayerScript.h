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

private:
    void Move();
    void Jump();
    void Shoot(Vec3 pos, Vec3 rot);
    void Aim(Vec3 rot);
};

