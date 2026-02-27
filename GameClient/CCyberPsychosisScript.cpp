#include "pch.h"
#include "CCyberPsychosisScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "CRigidBody.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "RenderMgr.h"

CCyberPsychosisScript::CCyberPsychosisScript() {}

CCyberPsychosisScript::~CCyberPsychosisScript() {}

void CCyberPsychosisScript::Begin() {
	auto rch = min(Random::Rand01() + 0.2f, 1.f) * 2.f - 1.f;
	auto alpha = min(Random::Rand01() + 0.4f, 1.f);
	mColor = Vec4(rch, -0.5f, -0.5f, alpha);

	mLifeTime = min(Random::Rand01() + 0.05f, 1.f) * 0.1f;

	SpriteRender()->SetEmit(true);
}

void CCyberPsychosisScript::Tick() {
	mElapsedTime += DT;
	if (mElapsedTime >= mLifeTime) SelfDestruct();

	SpriteRender()->SetAlbedo(mColor);
	SpriteRender()->SetGlitchEffect(true);

	auto dest = mTarget->Transform()->GetRelativePos() + mOffset;
	Transform()->SetRelativePos(dest);
}

void CCyberPsychosisScript::SelfDestruct() {
	TaskInfo info{};
	info.Type = TASK_TYPE::DESTROY_OBJECT;
	info.Param_0 = (DWORD_PTR)GetOwner();
	TaskMgr::GetInst()->AddTask(info);
}