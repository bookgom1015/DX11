#include "pch.h"
#include "CRelicScript.h"

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

CRelicScript::CRelicScript() 
	: CScript(SCRIPT_TYPE::RELICSCRIPT) {}

CRelicScript::~CRelicScript() {}

void CRelicScript::Begin() {
	auto rch = min(Random::Rand01() + 0.2f, 1.f) * 2.f - 1.f;
	auto alpha = min(Random::Rand01() + 0.4f, 1.f);

	SpriteRender()->SetEmit(true);
}

void CRelicScript::Tick() {
	mElapsedTime += DT;	

	//SpriteRender()->SetAlbedo(mColor);
	SpriteRender()->SetRelicEffect(true);

	auto dest = mTarget->Transform()->GetRelativePos();
	Transform()->SetRelativePos(dest);
}

void CRelicScript::SetAlbedo(const Vec4& color) {
	SpriteRender()->SetAlbedo(color);
}