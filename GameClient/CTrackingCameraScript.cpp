#include "pch.h"
#include "CTrackingCameraScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "CTransform.h"
#include "GameObject.h"

CTrackingCameraScript::CTrackingCameraScript() {}

CTrackingCameraScript::~CTrackingCameraScript() {}

void CTrackingCameraScript::Tick() {
	if (mTarget != nullptr) {
		static auto offset = Vec3(0.f, 300.f, 0.f);

		auto currPos = Transform()->GetRelativePos();
		auto targetPos = mTarget->Transform()->GetRelativePos();

		Vec3 newPos = Vec3(targetPos.x, targetPos.y, currPos.z);

		Transform()->SetRelativePos(newPos + offset);
	}
}

void CTrackingCameraScript::SetTarget(Ptr<GameObject> target) {
	mTarget = target;
}