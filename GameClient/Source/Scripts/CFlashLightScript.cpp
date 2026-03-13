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
	: CScript(SCRIPT_TYPE::FLASHLIGHTSCRIPT) {
	ADD_PROPERTY(CFlashLightScript, "Flashlight", mFlashLight, Property::Object);
	ADD_PROPERTY(CFlashLightScript, "Flashlight2", mFlashLight2, Property::Object);
}

CFlashLightScript::~CFlashLightScript() {}

void CFlashLightScript::TickOnce() {
	CScript::TickOnce();

	// 점광원
	mFlashLight = NEW GameObject;
	mFlashLight->SetName(L"Player_PointLight");
	mFlashLight->AddComponent(NEW CTransform);
	mFlashLight->AddComponent(NEW CLight2D);

	const auto& light = mFlashLight->Light2D();
	light->SetLightType(ELight::E_Point);
	light->SetLightColor(Vec3(1.f));
	light->SetIntensity(0.08f);
	light->SetRadius(250.f);
	light->Disable(false);

	CreateObject(mFlashLight.Get(), ELevelLayer::E_Light);

	// Spot light
	mFlashLight2 = NEW GameObject;
	mFlashLight2->SetName(L"Player_SpotLight");
	mFlashLight2->AddComponent(NEW CTransform);
	mFlashLight2->AddComponent(NEW CLight2D);

	const auto& light2 = mFlashLight2->Light2D();
	light2->SetLightType(ELight::E_Spot);
	light2->SetLightColor(Vec3(1.f));
	light2->SetIntensity(0.35f);
	light2->SetLightDir(Vec3(1.f, 0.f, 0.f));
	light2->SetRadius(1000.f);
	light2->SetAngle(DegToRad * 90.f);
	light2->Disable(false);

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

void CFlashLightScript::CleanUp() {
	DestroyObject(mFlashLight.Get());
	DestroyObject(mFlashLight2.Get());
}