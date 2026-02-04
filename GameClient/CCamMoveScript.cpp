#include "pch.h"
#include "CCamMoveScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "CTransform.h"


CCamMoveScript::CCamMoveScript()
{
}

CCamMoveScript::~CCamMoveScript()
{
}

void CCamMoveScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();

	Vec3 vFront = Transform()->GetDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetDir(DIR::RIGHT);


	//if (KEY_PRESSED(KEY::W))
	//	vPos.y += DT * 500.f;
	//if (KEY_PRESSED(KEY::S))
	//	vPos.y -= DT * 500.f;
	//if (KEY_PRESSED(KEY::A))
	//	vPos.x -= DT * 500.f;
	//if (KEY_PRESSED(KEY::D))
	//	vPos.x += DT * 500.f;

	if (1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos += vFront * 10.f;
	if (-1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos -= vFront * 10.f;


	if (KEY_PRESSED(KEY::W))
		vPos += vFront * 500.f * DT;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * 500.f * DT;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * 500.f * DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * 500.f * DT;

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		vRot.y += vMouseDir.x * DT * XM_2PI * 3.f;
		vRot.x += vMouseDir.y * DT * XM_2PI * 3.f;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);
}