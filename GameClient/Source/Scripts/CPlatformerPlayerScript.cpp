#include "pch.h"
#include "CPlatformerPlayerScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "AssetMgr.h"
#include "TaskMgr.h"
#include "EditorMgr.h"

#include "GameObject.h"
#include "CTransform.h"
#include "CRigidBody.h"

#include "CTargetMissileScript.h"
#include "CSandevistanScript.h"
#include "CCyberPsychosisScript.h"
#include "CRelicScript.h"

#include "SceneUI.h"

CPlatformerPlayerScript::CPlatformerPlayerScript() 
	: CScript(SCRIPT_TYPE::PLATFORMERPLAYERSCRIPT) {}

CPlatformerPlayerScript::~CPlatformerPlayerScript() {}

void CPlatformerPlayerScript::Begin() {
	// Muzzle flash
	mLight = new GameObject;
	mLight->SetName(L"MuzzleFlash");
	mLight->AddComponent(new CTransform);
	mLight->AddComponent(new CLight2D);

	decltype(auto) light = mLight->Light2D();
	light->SetLightType(LIGHT_TYPE::POINT);
	light->SetLightColor(Vec3(255.f / 255.f, 81.f / 255.f, 13.f / 255.f));
	light->SetIntensity(5.f);
	light->SetRadius(80.f);
	light->Disable(true);

	CreateObject(mLight.Get(), ELevelLayer::E_Light);
}

void CPlatformerPlayerScript::Tick() {
	auto editor = EditorMgr::GetInst()->FindUI("Scene");
	auto scene = static_cast<SceneUI*>(editor.Get());

	// 마우스 방면 방향 계산
	auto pos = Transform()->GetRelativePos();
	auto mpos = KeyMgr::GetInst()->GetMousePosOnScene();

	auto wpos = RenderMgr::GetInst()->GetPOVCamera()->Camera()->ScreenToWorld(
		mpos, scene->GetSceneSize());

	auto dir = (wpos - pos).Normalize();

	Move();
	Aim(Vec3(0.f, 0.f, atan2(dir.y, dir.x)));
	if (KEY_TAP(KEY::SPACE)) Jump();
	Shoot(pos + dir * 100.f, Vec3(0.f, 0.f, atan2(dir.y, dir.x) - XM_PIDIV2), dir);

	static const float coolTime = 0.05f;
	static float elapsedTime = 0.f;

	if (KEY_PRESSED(KEY::LSHIFT) && RigidBody()->GetActualVelocity().Length() > 0.f) {
		elapsedTime += DT;
		if (elapsedTime >= coolTime) {
			elapsedTime = 0.f;

			Sandevistan();
		}
	}
	else {
		elapsedTime = 0.f;
	}

	CyberPsychosis();
	Relic();

	Animation();
	Relocate();

	if (RigidBody()->IsOnGround()) {
		auto ground = RigidBody()->GetGround();
		auto groundPos = ground->Transform()->GetRelativePos();
	
		if (mGround == nullptr) mGroundPrevPos = groundPos;
	
		auto delta = groundPos - mGroundPrevPos;
	
		auto pos = Transform()->GetRelativePos();
		Transform()->SetRelativePos(pos + delta);
	
		mGroundPrevPos = groundPos;
		mGround = ground;
	}
	else {
		mGround = nullptr;
	}
}

void CPlatformerPlayerScript::Move() {
	if (KEY_PRESSED(KEY::LSHIFT))
		mSpeed = SprintSpeed;
	else
		mSpeed = WalkSpeed;

	Vec3 velocity{};
	if (KEY_PRESSED(KEY::A)) {
		static Vec3 dir(-1.f, 0.f, 0.f);
		velocity += dir * mSpeed;
	}
	if (KEY_PRESSED(KEY::D)) {
		static Vec3 dir(1.f, 0.f, 0.f);
		velocity += dir * mSpeed;
	}

	RigidBody()->SetVelocity(velocity);
}

void CPlatformerPlayerScript::Jump() {
	if (RigidBody()->IsOnGround()) {
		mbCanDoubleJump = TRUE;
		RigidBody()->AddImpulse(Vec3(0.f, 1000.f, 0.f));
	}
	else if (mbCanDoubleJump) {
		mbCanDoubleJump = FALSE;
		RigidBody()->AbsorbVerticleForce();
		RigidBody()->AddImpulse(Vec3(0.f, 1000.f, 0.f));
	}
}

void CPlatformerPlayerScript::Shoot(Vec3 pos, Vec3 rot, Vec3 dir) {
	static const float interval = 0.25f;
	static const float fInterval = 0.05f;

	if (mbFlashed) {
		mbFlashTimer += DT;
		if (mbFlashTimer >= fInterval) {
			mbFlashTimer = 0.f;
			mbFlashed = false;
			mLight->Light2D()->Disable(true);
		}
	}

	if (KEY_TAP(KEY::LBTN) || KEY_PRESSED(KEY::LBTN)) {
		if (KEY_PRESSED(KEY::LBTN)) {
			mFireElapsedTime += DT;
			if (mFireElapsedTime < interval)  return;
		}
		mFireElapsedTime = 0.f;

		// 미사일 생성
		GameObject* pObject = new GameObject;
		pObject->SetName(MakeUniqueName(L"Missile"));

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);
		pObject->AddComponent(new CCollider2D);
		pObject->AddComponent(new CTargetMissileScript);

		decltype(auto) trans = pObject->Transform();
		trans->SetRelativeScale(Vec3(10.f, 30.f, 1.f));
		trans->SetRelativeRot(rot);
		trans->SetRelativePos(pos);

		decltype(auto) renderer = pObject->MeshRender();
		renderer->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
		renderer->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

		CreateObject(pObject, ELevelLayer::E_Projectile);		

		mLight->Light2D()->Disable(false);
		mbFlashed = true;
	}
}

void CPlatformerPlayerScript::Aim(Vec3 rot) {
	auto arm = GetOwner()->GetChild(0);
	arm->Transform()->SetRelativeRot(rot);

	auto hand = arm->GetChild(0);

	auto handPos = hand->Transform()->GetWorldPos();
	auto armPos = arm->Transform()->GetWorldPos();

	auto offset = handPos - armPos;

	mLight->Transform()->SetRelativePos(handPos + offset);
}

void CPlatformerPlayerScript::Sandevistan() {
	GameObject* pEffect = new GameObject;
	pEffect->SetName(MakeUniqueName(L"SandevistanEffect"));
	pEffect->AddComponent(new CTransform);
	pEffect->AddComponent(new CSpriteRender);
	pEffect->AddComponent(new CSandevistanScript);

	decltype(auto) trans = pEffect->Transform();
	trans->SetRelativePos(Transform()->GetRelativePos());
	trans->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	auto sprite = FlipbookRender()->GetCurrentSprite();
	pEffect->SpriteRender()->SetSprite(sprite);

	CreateObject(pEffect, ELevelLayer::E_Particle);

	{
		auto entry = LogEntry{ 
			.Level = LogLevel::E_Warning, 
			.Message = "당신도\n산데비\n스탄 이\n에요?" };
		EditorMgr::GetInst()->AddLog(entry);
	}
	{
		auto entry = LogEntry{ 
			.Level = LogLevel::E_Critical, 
			.Message = "초보적인\n임플란트\n에요" };
		EditorMgr::GetInst()->AddLog(entry);
	}
}

void CPlatformerPlayerScript::CyberPsychosis() {
	if (KEY_TAP(KEY::Z)) mbCyberPsychosis = !mbCyberPsychosis;
	if (!mbCyberPsychosis) return;

	mCyberPsychosisElapsedTime += DT;
	if (mCyberPsychosisElapsedTime > mCyberPsychosisInterval) {
		mCyberPsychosisInterval = min(Random::Rand01() + 0.025f, 1.f) * 0.1f;
		mCyberPsychosisElapsedTime = 0.f;

		GameObject* pEffect = new GameObject;
		pEffect->SetName(MakeUniqueName(L"CyberPyschosisEffect"));
		pEffect->AddComponent(new CTransform);
		pEffect->AddComponent(new CSpriteRender);

		auto script = new CCyberPsychosisScript;
		pEffect->AddComponent(script);

		auto pos = Transform()->GetRelativePos();
		auto x = Random::Rand01() * 2.f - 1.f + 0.1f;
		auto y = Random::Rand01() * 2.f - 1.f + 0.1f;
		auto z = Random::Rand01() * 2.f - 1.f + 0.1f;
		auto offset = Vec3(x, y, z) * 50.f;

		script->SetTarget(GetOwner());
		script->SetOffset(offset);

		pEffect->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

		auto sprite = FlipbookRender()->GetCurrentSprite();
		pEffect->SpriteRender()->SetSprite(sprite);

		CreateObject(pEffect, ELevelLayer::E_Particle);
	}
}

void CPlatformerPlayerScript::Relic() {
	if (KEY_TAP(KEY::X)) {
		if (mbRelic) {
			mbRelic = false;

			if (mpRelic != nullptr) {
				TaskInfo info{};
				info.Type = TASK_TYPE::DESTROY_OBJECT;
				info.Param_0 = (DWORD_PTR)mpRelic;
				TaskMgr::GetInst()->AddTask(info);
			}
			if (mpRelic2 != nullptr) {
				TaskInfo info{};
				info.Type = TASK_TYPE::DESTROY_OBJECT;
				info.Param_0 = (DWORD_PTR)mpRelic2;
				TaskMgr::GetInst()->AddTask(info);
			}
		}
		else {
			mbRelic = true;

			{
				mpRelic = new GameObject;

				mpRelic->SetName(MakeUniqueName(L"RelicEffect1"));
				mpRelic->AddComponent(new CTransform);
				mpRelic->AddComponent(new CSpriteRender);

				auto script = new CRelicScript;
				mpRelic->AddComponent(script);

				script->SetAlbedo(Vec4(16.f / 255.f, 52.f / 255.f, 255.f / 255.f, 0.f));
				script->SetTarget(GetOwner());

				mpRelic->Transform()->SetRelativeScale(Vec3(105.f, 105.f, 1.f));

				auto sprite = FlipbookRender()->GetCurrentSprite();
				mpRelic->SpriteRender()->SetSprite(sprite);

				CreateObject(mpRelic, ELevelLayer::E_Particle);
			}
			{
				mpRelic2 = new GameObject;

				mpRelic2->SetName(MakeUniqueName(L"RelicEffect2"));
				mpRelic2->AddComponent(new CTransform);
				mpRelic2->AddComponent(new CSpriteRender);

				auto script = new CRelicScript;
				mpRelic2->AddComponent(script);

				script->SetAlbedo(Vec4(7.f / 255.f, 225.5f / 255.f, 255.f / 255.f, 0.f));
				script->SetTarget(GetOwner());

				mpRelic2->Transform()->SetRelativeScale(Vec3(110.f, 110.f, 1.f));

				auto sprite = FlipbookRender()->GetCurrentSprite();
				mpRelic2->SpriteRender()->SetSprite(sprite);

				CreateObject(mpRelic2, ELevelLayer::E_Particle);
			}
		}
	}
	else {
		if (mbRelic) {
			if (mpRelic != nullptr) {
				auto sprite = FlipbookRender()->GetCurrentSprite();
				mpRelic->SpriteRender()->SetSprite(sprite);
			}
			if (mpRelic2 != nullptr) {
				auto sprite = FlipbookRender()->GetCurrentSprite();
				mpRelic2->SpriteRender()->SetSprite(sprite);
			}
		}
	}
}

void CPlatformerPlayerScript::Animation() {
	static bool bRateChanged{};
	static float prevRate{ WalkSpeed };

	float rate = 15.f;
	if (abs(mSpeed - SprintSpeed) < 1e-4f) rate = 30.f;

	if (abs(rate - prevRate) >= 1e-4f) {
		bRateChanged = true;
		prevRate = rate;
	}

	if (RigidBody()->GetActualVelocity().x < -1e-4f) {
		if (bRateChanged || mIndex != 1) {
			FlipbookRender()->Play(1, rate, -1);
			mIndex = 1;
		}
	}
	else if (RigidBody()->GetActualVelocity().x > 1e-4f) {
		if (bRateChanged || mIndex != 2) {
			FlipbookRender()->Play(3, rate, -1);
			mIndex = 3;
		}
	}
	else {
		FlipbookRender()->Stop(0);
		mIndex = 0;
	}

	bRateChanged = true;
}

void CPlatformerPlayerScript::Relocate() {
	if (Transform()->GetRelativePos().y < -10000.f)
		Transform()->SetRelativePos({});
}