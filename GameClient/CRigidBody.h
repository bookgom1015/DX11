#pragma once
#include "CScript.h"

#include "GameObject.h"

class CRigidBody : public Component {
public:
    CRigidBody();
    virtual ~CRigidBody();

public:
    virtual void Begin() override;
    virtual void LateTick() override;
    virtual void FinalTick() override;

public:
    CLONE(CRigidBody);

    virtual void SaveToLevelFile(FILE* const _FileStream) override;
    virtual void LoadFromLevelFile(FILE* const _FileStream) override;

public:
    void AddForce(const Vec3& force);
    void AddImpulse(const Vec3& force);

    void SetVelocity(const Vec3& vel);
    void SetActualVelocity(const Vec3& vel);
    Vec3 GetVelocity() const { return mVelocity; }
    Vec3 GetActualVelocity() const { return mVelocity + mManualVelocity; }

    bool IsOnGround() const noexcept { return mbGrounded; }
    Ptr<GameObject> GetGround() const { return mGround; }

    void AbsorbForce() {
        mVelocity = {};
        mManualVelocity = {};
    }

    void AbsorbVerticleForce() { 
        mVelocity.y = 0.f;
        mManualVelocity.y = 0.f;
    }

    void SetGravityCoefficient(float coeff) { mGravityCoeff = coeff; }

private:
    void Resolve();
    void PhysicsStep(float dt);
    void ApplyGravity();

    void OnBeginOverlap(CollisionData pOwn, CollisionData pOther);
    void OnOverlap(CollisionData pOwn, CollisionData pOther);
    void OnEndOverlap(CollisionData pOwn, CollisionData pOther);

private:
    Vec3 mVelocity{};
    Vec3 mManualVelocity{};
    Vec3 mAccleration{};

    float mDamping{ 0.98f };

    bool mbGrounded{};
    Ptr<GameObject> mGround;

    float mGravityCoeff{ 1.f };

    float mAccumulator{};

    bool mbKinematic{};
    Vec3 mPrevPos{};
};

