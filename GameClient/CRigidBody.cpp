#include "pch.h"
#include "CRigidBody.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "CMissileScript.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "RenderMgr.h"

namespace {
	const Vec3 gGravityForce = Vec3(0.f, -9.8f * 160.f, 0.f);
}

CRigidBody::CRigidBody() : Component(COMPONENT_TYPE::RIGIDBODY) {}

CRigidBody::~CRigidBody() {}

void CRigidBody::Begin() {
	Collider2D()->AddDynamicBeginOverlap(
		this, (COLLISION_EVENT_COMP)&CRigidBody::OnBeginOverlap);
	Collider2D()->AddDynamicEndOverlap(
		this, (COLLISION_EVENT_COMP)&CRigidBody::OnEndOverlap);
}

void CRigidBody::FinalTick() {
	if (!mbGrounded) ApplyGravity();

	Resolve();
}

void CRigidBody::AddForce(const Vec3& force) {
	mAccleration += force;
}

void CRigidBody::AddImpulse(const Vec3& force) {
	mVelocity += force;
}

void CRigidBody::Resolve() {
	auto currPos = Transform()->GetRelativePos();

	auto newPos = currPos + mVelocity * DT;
	mVelocity += mAccleration * DT;

	mAccleration = {};

	Transform()->SetRelativePos(newPos);
}

void CRigidBody::ApplyGravity() {
	mAccleration += gGravityForce;
}

void CRigidBody::OnBeginOverlap(CCollider2D* pOwn, CCollider2D* pOther) {
	if (pOther->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		mbGrounded = true;
		mVelocity = {};
	}
}

void CRigidBody::OnEndOverlap(CCollider2D* pOwn, CCollider2D* pOther) {
	if (pOther->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) 
		mbGrounded = false;
}