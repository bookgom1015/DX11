#include "pch.h"
#include "CParticleScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "CTargetMissileScript.h"
#include "CRigidBody.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "RenderMgr.h"

CParticleScript::CParticleScript() 
	: CScript(SCRIPT_TYPE::PARTICLESCRIPT) {
	ADD_PROPERTY(CParticleScript, "FromColor", mFromColor, Property::Vec3);
	ADD_PROPERTY(CParticleScript, "ToColor", mToColor, Property::Vec3);
}

CParticleScript::~CParticleScript() {}

void CParticleScript::Begin() {
	Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&CParticleScript::BeginOverlap);
	Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&CParticleScript::Overlap);
	Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&CParticleScript::EndOverlap);

	RigidBody()->SetGravityCoefficient(6.f);
}

void CParticleScript::Tick() {
	static float lifeTime = 4.f;

	mElapsedTime += DT;

	auto t = min(mElapsedTime / lifeTime, 1.f);
	auto color = (1.f - t) * mFromColor + t * mToColor;

	BillboardRender()->SetAlbedo(color);

	if (mElapsedTime > lifeTime) {
		//Transform()->SetRelativePos(Vec3(0.f, -10000.f, 0.f));
		Destroy();
	}
}

void CParticleScript::BeginOverlap(CollisionData pOwner, CollisionData pOther) {
	if (pOther.Collider->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		auto normal = pOther.HitNormal;	
		auto vel = RigidBody()->GetVelocity();	
		auto reflect = Reflect(vel, normal);
		auto damping = 0.5f;
		auto mag = reflect.Length();	

		if (mag < 100.f) reflect = {};
	
		RigidBody()->SetActualVelocity(reflect * damping);
	}
}

void CParticleScript::Overlap(CollisionData pOwner, CollisionData pOther) {
	if (pOther.Collider->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground) {
		RigidBody()->SetActualVelocity(RigidBody()->GetVelocity() * 0.98f);
	}
}

void CParticleScript::EndOverlap(CollisionData pOwner, CollisionData pOther) {

}