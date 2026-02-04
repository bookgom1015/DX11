#include "pch.h"
#include "CPlayerScript.h"

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

CPlayerScript::CPlayerScript()
	: m_Limit(0.f)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Tick()
{
	Move();

	Shoot();

	if (KEY_PRESSED(KEY::X))
	{
		MeshRender()->GetMtrl()->SetScalar(INT_0, 1);
	}
	else
	{
		MeshRender()->GetMtrl()->SetScalar(INT_0, 0);
	}

	Ptr<GameObject> pChild = GetOwner()->GetChild(0);

	Vec3 vRelativePos = pChild->Transform()->GetRelativePos();
	Vec3 vWorldPos = pChild->Transform()->GetWorldPos();


	//Ptr<CCamera> pCam = RenderMgr::GetInst()->GetPOVCamera();
	//Vec3 vCamPos = pCam->Transform()->GetPos();
	//Vec3 vMyPos = Transform()->GetPos();
	//Vec3 vDirToCam = vCamPos - vMyPos;
	//vDirToCam.Normalize();

	//Vec3 vBot = Vec3(vDirToCam.x, 0.f, vDirToCam.z).Normalize();
	//Vec3 vFront = Vec3(0.f, 0.f, -1.f);
	//	
	//float YDot = vBot.Dot(vFront);
	//float YAngle = acosf(YDot);

	//if (vFront.Cross(vBot).y < 0.f)
	//	YAngle = XM_2PI - YAngle;
	//	
	//float XDot = vBot.Dot(vDirToCam);
	//if (XDot > 1.f) XDot = 1.f;
	//if (XDot < -1.f) XDot = -1.f;

	//float XAngle = acosf(XDot);
	//if(vCamPos.y < vMyPos.y)
	//	XAngle = XM_2PI - XAngle;

	//Transform()->SetRotation(Vec3(0.f, YAngle, 0.f));
	//Transform()->SetRotation(Vec3(XAngle, 0.f, 0.f));
	//Transform()->SetRotation(Vec3(XAngle, YAngle, 0.f));
}

void CPlayerScript::Move()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vScale = Transform()->GetRelativeScale();
	Vec3 vRotation = Transform()->GetRelativeRot();

	Vec3 vUp = Transform()->GetDir(DIR::UP);
	Vec3 vDown = -vUp;

	if (KEY_PRESSED(KEY::UP))
		vPos += vUp * 100.f * DT;
	if (KEY_PRESSED(KEY::DOWN))
		vPos += vDown * 100.f * DT;

	if (KEY_PRESSED(KEY::RIGHT))
		vRotation.z -= XM_PI * DT;
	if (KEY_PRESSED(KEY::LEFT))
		vRotation.z += XM_PI * DT;

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeScale(vScale);
	Transform()->SetRelativeRot(vRotation);
}

void CPlayerScript::Shoot()
{
	if (KEY_TAP(KEY::SPACE))
	{
		// 미사일 역할의 오브젝트 생성
		GameObject* pObject = new GameObject;
		pObject->SetName(L"Missile");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);
		pObject->AddComponent(new CCollider2D);

		Ptr<CMissileScript> pMissileScript = new CMissileScript;
		pMissileScript->SetTarget(m_Target);
		pObject->AddComponent(pMissileScript.Get());

		Vec3 vMyPos = Transform()->GetRelativePos();
		Vec3 vMyScale = Transform()->GetRelativeScale();
		Vec3 vRotation = Transform()->GetRelativeRot();
		Vec3 vUp = Transform()->GetDir(DIR::UP);

		pObject->Transform()->SetRelativePos(vMyPos + vMyScale * 0.5f * vUp);
		pObject->Transform()->SetRelativeScale(Vec3(10.f, 30.f, 1.f));
		pObject->Transform()->SetRelativeRot(vRotation);

		pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
		pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

		CreateObject(pObject, 4);


		/*	TaskInfo info = {};
		info.Type = TASK_TYPE::DESTROY_OBJECT;
		info.Param_0 = (DWORD_PTR)GetOwner();
		TaskMgr::GetInst()->AddTask(info);*/
	}

	if (KEY_TAP(KEY::SPACE))
	{
		/*DrawDebugRect(Transform()->GetRelativePos()
					, Transform()->GetRelativeScale() * Vec3(2.f, 2.f, 2.f)
					, Vec3(0.f, 0.f, 0.f), Vec4(0.f, 1.f, 0.f, 1.f), 1.f);*/

		//DrawDebugRect(Transform()->GetRelativePos(), 100.f, Vec4(1.f, 0.f, 0.f, 1.f), 2.f);
		DrawDebugRect(Transform()->GetWorldMat(), Vec4(1.f, 0.f, 0.f, 1.f), 2.f);
	}	
}

