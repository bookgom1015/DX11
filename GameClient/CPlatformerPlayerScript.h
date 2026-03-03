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
    void Shoot(Vec3 pos, Vec3 rot, Vec3 dir);
    void Aim(Vec3 rot);

    void Sandevistan();
    void CyberPsychosis();
    void Relic();
    void Animation();
    void Relocate();

private:
    bool mbCanDoubleJump{ TRUE };

    const float WalkSpeed = 650.f;
    const float SprintSpeed = 1000.f;
    float mSpeed = 500.f;

    unsigned mIndex{};

    bool mbCyberPsychosis{};
    float mCyberPsychosisElapsedTime{};
    float mCyberPsychosisInterval{};

    GameObject* mpRelic{};
    GameObject* mpRelic2{};
    bool mbRelic{};

    bool mbFiring{};
    float mFireElapsedTime{};

    Ptr<GameObject> mLight{};
    bool mbFlashed{};
    float mbFlashTimer{};

    Ptr<GameObject> mGround{};
    Vec3 mGroundPrevPos{};
};

