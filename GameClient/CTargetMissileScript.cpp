#include "pch.h"
#include "CTargetMissileScript.h"

#include "GameObject.h"
#include "TimeMgr.h"
#include "CCollider2D.h"
#include "TaskMgr.h"

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
	Transform()->SetRelativePos(pos + dir * speed * DT);

	//if (IsValid(mTarget))
	//{
	//	Vec3 vTargetPos = mTarget->Transform()->GetWorldPos();
	//	Vec3 vPos = Transform()->GetRelativePos();
	//
	//	static Vec3 vDir = Vec3(0.f, 0.f, 0.f);
	//	if (vDir == Vec3(0.f, 0.f, 0.f))
	//	{
	//		vDir = vTargetPos - vPos;
	//		vDir.Normalize();
	//	}
	//	vPos += vDir * 200.f * DT;
	//
	//	//Vec3 vDirToTarget = vTargetPos - vPos;
	//	//vDirToTarget.Normalize();
	//	//vPos += vDirToTarget * 200.f * DT;
	//
	//	Transform()->SetRelativePos(vPos);
	//}
}

void CTargetMissileScript::BeginOverlap(CCollider2D* pOwner, CCollider2D* pOther) {
	if (pOther->GetOwner()->GetLayerIndex() == ELevelLayer::E_Ground ||
			pOther->GetOwner()->GetLayerIndex() == ELevelLayer::E_Obstacle) {
		Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
		SelfDestruct();
	}
}

void CTargetMissileScript::Overlap(CCollider2D* pOwner, CCollider2D* pOther) {

}

void CTargetMissileScript::EndOverlap(CCollider2D* pOwner, CCollider2D* pOther) {

}

void CTargetMissileScript::SelfDestruct() {
	TaskInfo info{};
	info.Type = TASK_TYPE::DESTROY_OBJECT;
	info.Param_0 = (DWORD_PTR)GetOwner();
	TaskMgr::GetInst()->AddTask(info);
}