#include "pch.h"
#include "CPlatformerPlayerScript.h"

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

CPlatfomerPlayerScript::CPlatfomerPlayerScript() {}

CPlatfomerPlayerScript::~CPlatfomerPlayerScript() {}

void CPlatfomerPlayerScript::Begin() {}

void CPlatfomerPlayerScript::Tick() {
	// 마우스 방면 방향 계산
	auto pos = Transform()->GetRelativePos();
	auto mpos = KeyMgr::GetInst()->GetMousePos();

	auto wpos = LevelMgr::GetInst()->GetMainCam()->Camera()->ScreenToWorld(mpos);

	auto dir = (wpos - pos).Normalize();

	Move();
	Aim(Vec3(0.f, 0.f, atan2(dir.y, dir.x)));
	if (KEY_TAP(KEY::SPACE)) Jump();
	if (KEY_TAP(KEY::LBTN)) Shoot(pos + dir * 100.f, Vec3(0.f, 0.f, atan2(dir.y, dir.x) - XM_PIDIV2));

}

void CPlatfomerPlayerScript::Move() {
	static float speed = 1000.f;

	Vec3 velocity{};
	if (KEY_PRESSED(KEY::A)) {
		static Vec3 dir(-1.f, 0.f, 0.f);
		velocity += dir * speed;
	}
	if (KEY_PRESSED(KEY::D)) {
		static Vec3 dir(1.f, 0.f, 0.f);
		velocity += dir * speed;
	}

	RigidBody()->SetVelocity(velocity);
}

void CPlatfomerPlayerScript::Jump() {
	if (RigidBody()->IsOnGround()) 
		RigidBody()->AddImpulse(Vec3(0.f, 1000.f, 0.f));
}

void CPlatfomerPlayerScript::Shoot(Vec3 pos, Vec3 rot) {
	// 미사일 생성
	GameObject* pObject = new GameObject;
	pObject->SetName(L"Missile");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CTargetMissileScript);

	pObject->Transform()->SetRelativeScale(Vec3(10.f, 30.f, 1.f));
	pObject->Transform()->SetRelativeRot(rot);
	pObject->Transform()->SetRelativePos(pos);
	//pObject->Transform()->SetRelativeRot(vRotation);

	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	CreateObject(pObject, ELevelLayer::E_Projectile);
}

void CPlatfomerPlayerScript::Aim(Vec3 rot) {
	auto arm = GetOwner()->GetChild(0);
	arm->Transform()->SetRelativeRot(rot);
}