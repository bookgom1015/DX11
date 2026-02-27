#include "pch.h"
#include "CElevationScript.h"

#include "TimeMgr.h"

CElevationScript::CElevationScript() {}

CElevationScript::~CElevationScript() {}

void CElevationScript::Begin() {
	mOriginalPos = Transform()->GetRelativePos();

	mElapsedTime = Random::Rand01() * 100.f;
}

void CElevationScript::Tick() {
	mElapsedTime += DT;

	float t = sin(mElapsedTime * mSpeed);

	Vec3 finalPos = mOriginalPos * (1 - t) + mStopOverPos * t;

	Transform()->SetRelativePos(finalPos);
}