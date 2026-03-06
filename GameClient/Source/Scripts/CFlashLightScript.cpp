#include "pch.h"
#include "CFlashLightScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "EditorMgr.h"

#include "GameObject.h"

#include "CTransform.h"
#include "CRigidBody.h"

#include "SceneUI.h"

CFlashLightScript::CFlashLightScript() 
	: CScript(SCRIPT_TYPE::FLASHLIGHTSCRIPT) {}

CFlashLightScript::~CFlashLightScript() {}

void CFlashLightScript::Begin() {
	// 점광원
	mFlashLight = new GameObject;
	mFlashLight->SetName(L"Player_PointLight");
	mFlashLight->AddComponent(new CTransform);
	mFlashLight->AddComponent(new CLight2D);
	
	mFlashLight->Light2D()->SetLightType(ELight::E_Point);
	mFlashLight->Light2D()->SetLightColor(Vec3(1.f));
	mFlashLight->Light2D()->SetIntensity(0.08f);
	mFlashLight->Light2D()->SetRadius(250.f);

	CreateObject(mFlashLight.Get(), ELevelLayer::E_Light);

	// Spot light
	mFlashLight2 = new GameObject;
	mFlashLight2->SetName(L"Player_SpotLight");
	mFlashLight2->AddComponent(new CTransform);
	mFlashLight2->AddComponent(new CLight2D);

	mFlashLight2->Light2D()->SetLightType(ELight::E_Spot);
	mFlashLight2->Light2D()->SetLightColor(Vec3(1.f));
	mFlashLight2->Light2D()->SetIntensity(0.35f);
	mFlashLight2->Light2D()->SetLightDir(Vec3(1.f, 0.f, 0.f));
	mFlashLight2->Light2D()->SetRadius(1000.f);
	mFlashLight2->Light2D()->SetAngle(DegToRad * 90.f);

	CreateObject(mFlashLight2.Get(), ELevelLayer::E_Light);
}

void CFlashLightScript::Tick() {	
	auto editor = EditorMgr::GetInst()->FindUI("Scene");
	auto scene = static_cast<SceneUI*>(editor.Get());

	// 마우스 방면 방향 계산
	auto pos = Transform()->GetRelativePos();
	auto mpos = KeyMgr::GetInst()->GetMousePosOnScene();

	auto wpos = RenderMgr::GetInst()->GetPOVCamera()->Camera()->ScreenToWorld(
		mpos, scene->GetSceneSize());

	auto dir = (wpos - pos).Normalize();

	auto mat = DirectX::XMMatrixRotationAxis(Vec4(0.f, 0.f, 1.f, 0.f), atan2(dir.y, dir.x));
	Vec3 newPos = XMVector3TransformCoord(Vec3(80.f, 0.f, 0.f), mat);

	auto dest = GetOwner()->Transform()->GetRelativePos();
	mFlashLight->Transform()->SetRelativePos(dest);
	mFlashLight2->Transform()->SetRelativePos(dest + newPos);
	mFlashLight2->Light2D()->SetLightDir(dir);
}