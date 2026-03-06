#include "pch.h"
#include "CRigidBody.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "EditorMgr.h"

#include "GameObject.h"

#include "CTransform.h"

namespace {
	const Vec3 gGravityForce = Vec3(0.f, -9.8f * 160.f, 0.f);

	const float gFixedDT = 1.f / 60.f;
	const int gMaxSteps = 8; // 폭주 방지
}

CRigidBody::CRigidBody() : Component(EComponent::E_Rigidbody) {}

CRigidBody::~CRigidBody() {}

void CRigidBody::Begin() {
	Collider2D()->AddDynamicBeginOverlap(
		this, (COLLISION_EVENT_COMP)&CRigidBody::OnBeginOverlap);
	Collider2D()->AddDynamicOverlap(
		this, (COLLISION_EVENT_COMP)&CRigidBody::OnOverlap);
	Collider2D()->AddDynamicEndOverlap(
		this, (COLLISION_EVENT_COMP)&CRigidBody::OnEndOverlap);
}

void CRigidBody::LateTick() {
	Resolve();
}

void CRigidBody::FinalTick() {}

void CRigidBody::AddForce(const Vec3& force) {
	mAccleration += force;
}

void CRigidBody::AddImpulse(const Vec3& force) {
	mVelocity += force;
}

void CRigidBody::SetVelocity(const Vec3& vel) {
	mManualVelocity = vel;
}

void CRigidBody::SetActualVelocity(const Vec3& vel) {
	mVelocity = vel;
}

void CRigidBody::Resolve() {
	if (mbKinematic) {
		auto pos = GetOwner()->Transform()->GetRelativePos();

		auto delta = pos - mPrevPos;
		mVelocity = delta;

		mPrevPos = pos;
	}
	else {
		mAccumulator += DT;

		int steps = 0;
		while (mAccumulator >= gFixedDT && steps < gMaxSteps) {
			PhysicsStep(gFixedDT);
			mAccumulator -= gFixedDT;
			++steps;
		}
	}
}

void CRigidBody::PhysicsStep(float dt) {
	if (!mbGrounded) mAccleration += gGravityForce;

	mVelocity += mAccleration * dt;

	if (mbGrounded) mVelocity.x *= expf(-20.f * dt);

	auto currPos = Transform()->GetRelativePos();
	auto newPos = currPos + (mVelocity + mManualVelocity) * dt;

	Transform()->SetRelativePos(newPos);

	mAccleration = {};
}

void CRigidBody::ApplyGravity() {
}

void CRigidBody::OnBeginOverlap(CollisionData pOwn, CollisionData pOther) {
	if (pOther.Collider->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		auto dot = pOther.HitNormal.Dot(Vec3(0.f, 1.f, 0.f));
		if (pOwn.Collider->GetOwner()->GetLayerIndex() != ELevelLayer::E_Particle) {
			if (dot > 0.9f) {
				mbGrounded = true;
				mGround = pOther.Collider->GetOwner();
				mVelocity.y = max(0.f, mVelocity.y);
			}
			else if (dot < -0.9f) {
				mVelocity.y = min(0.f, mVelocity.y);
			}
		}
	}
}

void CRigidBody::OnOverlap(CollisionData pOwn, CollisionData pOther) {
	if (pOther.Collider->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		if (pOwn.Collider->GetOwner()->GetLayerIndex() != ELevelLayer::E_Particle) {
			auto ownIdx = pOwn.Collider->GetOwner()->GetLayerIndex();
			auto otherIdx = pOther.Collider->GetOwner()->GetLayerIndex();

			if (ownIdx < otherIdx) pOther.HitNormal *= -1.f;
			auto dot = pOther.HitNormal.Dot(Vec3(0.f, 1.f, 0.f));

			if (dot > 0.9f) {
				mbGrounded = true;
				mGround = pOther.Collider->GetOwner();
				mVelocity.y = max(0.f, mVelocity.y);
			}
		}

		auto pos = Transform()->GetRelativePos();
		auto obsPos = pOther.Collider->Transform()->GetRelativePos();

		Vec2 scale = Transform()->GetRelativeScale() * 0.5f * Collider2D()->GetScale();
		Vec2 offset = scale * Collider2D()->GetOffset() * -1.f;

		Vec2 obsScale = pOther.Collider->Transform()->GetRelativeScale() 
			* 0.5f * pOther.Collider->Collider2D()->GetScale();
		Vec2 obsOffset = obsScale * pOther.Collider->Collider2D()->GetOffset() * -1.f;

		auto posMin = pos - Vec3(scale.x + offset.x, scale.y + offset.y, 0.f);
		auto posMax = pos + Vec3(scale.x + offset.x, scale.y + offset.y, 0.f);

		auto obsPosMin = obsPos 
			- Vec3(obsScale.x + obsOffset.x, obsScale.y + obsOffset.y, 0.f);
		auto obsPosMax = obsPos 
			+ Vec3(obsScale.x + obsOffset.x, obsScale.y + obsOffset.y, 0.f);
				
		if ((posMax.x >= obsPosMin.x && obsPosMax.x >= posMin.x) 
			&& (posMax.y >= obsPosMin.y && obsPosMax.y >= posMin.y)) {
			const float moveRight = obsPosMax.x - posMin.x;
			const float moveLeft = obsPosMin.x - posMax.x;
			const float moveUp = obsPosMax.y - posMin.y;
			const float moveDown = obsPosMin.y - posMax.y;

			float minMove = moveRight;
			Vec2 mtv(moveRight, 0.f);

			if (fabsf(moveLeft) < fabsf(minMove)) { 
				minMove = moveLeft;  
				mtv = Vec2(moveLeft, 0.f); 
			}
			if (fabsf(moveUp) < fabsf(minMove)) { 
				minMove = moveUp;    
				mtv = Vec2(0.f, moveUp); 
			}
			if (fabsf(moveDown) < fabsf(minMove)) { 
				minMove = moveDown;  
				mtv = Vec2(0.f, moveDown); 
			}

			pos.x += mtv.x;
			pos.y += mtv.y;		
			
			Transform()->SetRelativePos(pos);

			//const float DampScale = 0.5f;
			//if (mtv.Length() > DampScale) {
			//	pos.x += mtv.x;
			//	pos.y += mtv.y;
			//
			//	auto damp = mtv;
			//	damp.Normalize();
			//	damp *= DampScale;
			//
			//	pos.x -= damp.x;
			//	pos.y -= damp.y;
			//
			//	Transform()->SetRelativePos(pos);
			//}
		}
	}
}

void CRigidBody::OnEndOverlap(CollisionData pOwn, CollisionData pOther) {
	if (pOther.Collider->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		if (pOwn.Collider->GetOwner()->GetLayerIndex() != ELevelLayer::E_Particle) {
			mbGrounded = false;
			mGround = nullptr;
		}
	}	
}