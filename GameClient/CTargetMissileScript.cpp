#include "pch.h"
#include "CTargetMissileScript.h"
#include "CParticleScript.h"

#include "GameObject.h"
#include "TimeMgr.h"
#include "CCollider2D.h"
#include "TaskMgr.h"
#include "LevelMgr.h"
#include "AssetMgr.h"

CTargetMissileScript::CTargetMissileScript() {}

CTargetMissileScript::~CTargetMissileScript() {}

void CTargetMissileScript::Begin() {
	Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&CTargetMissileScript::BeginOverlap);
	Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&CTargetMissileScript::Overlap);
	Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&CTargetMissileScript::EndOverlap);
}

void CTargetMissileScript::Tick() {
	mElapsedTime += DT;
	if (mElapsedTime > 2.f) {
		SelfDestruct();
		return;
	}

	auto pos = Transform()->GetRelativePos();
	auto dir = Transform()->GetDir(DIR::UP);

	static float speed = 2400.f;
	auto newPos = pos + dir * speed * DT;
	Transform()->SetRelativePos(newPos);
}

void CTargetMissileScript::BeginOverlap(CollisionData pOwner, CollisionData pOther) {
	auto index = pOther.Collider->GetOwner()->GetLayerIndex();
	if (index == ELevelLayer::E_Ground) {
		auto origin = Transform()->GetRelativePos();

		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
		SelfDestruct();

		CreateParticles(origin, false);
	}
	else if (index == ELevelLayer::E_Enemy) {
		auto origin = Transform()->GetRelativePos();

		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
		SelfDestruct();

		CreateParticles(origin, true);

		auto ownerDir = Transform()->GetDir(DIR::UP);
		pOther.Collider->GetOwner()->RigidBody()->AddImpulse(ownerDir * 100.f);
	}
}

void CTargetMissileScript::Overlap(CollisionData pOwner, CollisionData pOther) {

}

void CTargetMissileScript::EndOverlap(CollisionData pOwner, CollisionData pOther) {

}

void CTargetMissileScript::SelfDestruct() {
	TaskInfo info{};
	info.Type = TASK_TYPE::DESTROY_OBJECT;
	info.Param_0 = (DWORD_PTR)GetOwner();
	TaskMgr::GetInst()->AddTask(info);
}

void CTargetMissileScript::CreateParticles(Vec3 pos, bool blood) {
	for (unsigned i = 0; i < 10; ++i) {
		GameObject* pParticle = new GameObject;

		auto name = std::format(L"Particle_{}", i);
		pParticle->SetName(MakeUniqueName(name));

		pParticle->AddComponent(new CTransform);
		pParticle->AddComponent(new CBillboardRender);
		pParticle->AddComponent(new CCollider2D);
		pParticle->AddComponent(new CRigidBody);

		auto particle = new CParticleScript;
		if (blood) {
			particle->SetFromColor(Vec3(1.f, 0.f, 0.f));
			particle->SetToColor(Vec3(0.15f, 0.f, 0.f));
		}
		else {
			auto color = Vec3(227.f / 255.f, 70.f / 255.f, 15.f / 255.f);
			particle->SetFromColor(color * 8.f);
			particle->SetToColor(color * 0.1f);
		}
		pParticle->AddComponent(particle);

		float x = Random::Rand01() * 2.f - 1.f;
		float y = Random::Rand01() * 2.f - 1.f;
		float z = Random::Rand01() * 2.f - 1.f;
		auto dir = Vec3(x, y, z).Normalize();

		auto rot = Vec3(0.f, 0.f, atan2(dir.y, dir.x));

		pParticle->Transform()->SetRelativePos(pos);
		pParticle->Transform()->SetRelativeRot(rot);
		pParticle->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 0.f));
		pParticle->BillboardRender()->SetBillboardScale(Vec2(10.f, 10.f));

		pParticle->RigidBody()->AddImpulse(dir * 1000.f);

		CreateObject(pParticle, ELevelLayer::E_Particle);
	}
}