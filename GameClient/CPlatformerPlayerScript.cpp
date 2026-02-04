#include "pch.h"
#include "CPlatformerPlayerScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "CMissileScript.h"
#include "CRigidBody.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"
#include "RenderMgr.h"

CPlatfomerPlayerScript::CPlatfomerPlayerScript() {}

CPlatfomerPlayerScript::~CPlatfomerPlayerScript() {}

void CPlatfomerPlayerScript::Begin() {}

void CPlatfomerPlayerScript::Tick() {
	if (KEY_TAP(KEY::SPACE)) {
		if (RigidBody()->IsOnGround())
			RigidBody()->AddImpulse(Vec3(0.f, 1000.f, 0.f));
	}
}