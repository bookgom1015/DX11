#include "pch.h"
#include "CCamMoveScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "CTransform.h"
#include "CCamera.h"

CCamMoveScript::CCamMoveScript() {}

CCamMoveScript::~CCamMoveScript() {}

void CCamMoveScript::Tick() {
	if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
		MovePerspective();
	else
		MoveOrthographic();
}

void CCamMoveScript::MovePerspective() {
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();

	Vec3 vFront = Transform()->GetDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetDir(DIR::RIGHT);


	if (1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos += vFront * 10.f;
	if (-1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos -= vFront * 10.f;


	if (KEY_PRESSED(KEY::W))
		vPos += vFront * 500.f * E_DT;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * 500.f * E_DT;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * 500.f * E_DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * 500.f * E_DT;

	if (KEY_PRESSED(KEY::RBTN))	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		vRot.y += vMouseDir.x * E_DT * XM_2PI * 3.f;
		vRot.x += vMouseDir.y * E_DT * XM_2PI * 3.f;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);
}

void CCamMoveScript::MoveOrthographic() {
	Vec3 vPos = Transform()->GetRelativePos();	

	if (KEY_PRESSED(KEY::W))
		vPos.y += E_DT * 500.f;
	if (KEY_PRESSED(KEY::S))
		vPos.y -= E_DT * 500.f;
	if (KEY_PRESSED(KEY::A))
		vPos.x -= E_DT * 500.f;
	if (KEY_PRESSED(KEY::D))
		vPos.x += E_DT * 500.f;

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
}
