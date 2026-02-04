#pragma once
#include "CScript.h"

#include "GameObject.h"

class CRigidBody : public Component {
public:
    CRigidBody();
    virtual ~CRigidBody();

public:
    virtual void Begin() override;
    virtual void FinalTick() override;

public:
    void AddForce(const Vec3& force);
    void AddImpulse(const Vec3& force);

    void SetVelocity(const Vec3& vel);

    bool IsOnGround() const noexcept { return mbGrounded; }

private:
    void Resolve();
    void ApplyGravity();

    void OnBeginOverlap(CCollider2D* pOwn, CCollider2D* pOther);
    void OnOverlap(CCollider2D* pOwn, CCollider2D* pOther);
    void OnEndOverlap(CCollider2D* pOwn, CCollider2D* pOther);

private:
    Vec3 mVelocity{};
    Vec3 mManualVelocity{};
    Vec3 mAccleration{};

    float mDamping{ 0.98f };

    bool mbGrounded{};
};

