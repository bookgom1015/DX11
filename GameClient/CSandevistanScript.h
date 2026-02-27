#pragma once
#include "CScript.h"

#include "GameObject.h"

class CSandevistanScript : public CScript {
public:
    CSandevistanScript();
    virtual ~CSandevistanScript();

public:
    virtual void Begin() override;
    virtual void Tick() override;

private:
    void SelfDestruct();

private:
    float mElapsedTime{};
};

