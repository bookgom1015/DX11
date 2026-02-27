#include "pch.h"
#include "CPlatformerPlayerScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "CSandevistanScript.h"
#include "CRigidBody.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "RenderMgr.h"

CSandevistanScript::CSandevistanScript() {}

CSandevistanScript::~CSandevistanScript() {}

void CSandevistanScript::Begin() {
	SpriteRender()->SetEmit(true);
}

void CSandevistanScript::Tick() {
	static const float lifeTime = 2.5f;
	static Vec4 colorStages[]{
		Vec4(0.f, 1.f, 0.f, 0.7f),
		Vec4(1.f, 1.f, 0.f, 0.6f),
		Vec4(0.f, 1.f, 1.f, 0.6f),
		Vec4(0.f, 0.f, 1.f, 0.5f),
		Vec4(1.f, 0.f, 0.f, 0.4f)
	};
	static float weightStages[]{
		0.18f,
		0.36f,
		0.54f,
		0.72f,
		0.9f
	};

	mElapsedTime += DT;
	if (mElapsedTime >= lifeTime) SelfDestruct();

	float weight = mElapsedTime / lifeTime;

	Vec4 finalColor = colorStages[0];
	for (size_t i = 0, end = _countof(colorStages) - 1; i < end; ++i) {
		size_t currStage = i;
		size_t nextStage = i + 1;

		auto currW = weightStages[currStage];
		auto nextW = weightStages[nextStage];

		if (weight >= weightStages[end]) {
			finalColor = colorStages[end];
			break;
		}
		else if (weight >= currW && weight <= nextW) {
			auto currColor = colorStages[currStage];
			auto nextColor = colorStages[nextStage];

			auto t = (weight - currW) / (nextW - currW);
			auto invT = 1.f - t;

			finalColor = invT * currColor + t * nextColor;
			break;
		}
	}

	SpriteRender()->SetAlbedo(finalColor);
}

void CSandevistanScript::SelfDestruct() {
	TaskInfo info{};
	info.Type = TASK_TYPE::DESTROY_OBJECT;
	info.Param_0 = (DWORD_PTR)GetOwner();
	TaskMgr::GetInst()->AddTask(info);
}