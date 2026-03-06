#include "pch.h"
#include "CCamMoveScript.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "EditorMgr.h"

#include "CTransform.h"
#include "CCamera.h"

CCamMoveScript::CCamMoveScript() 
	: CScript(-1), m_MousePrevPos{} {}

CCamMoveScript::~CCamMoveScript() {}

void CCamMoveScript::Tick() {
	if (EProjection::E_Perspective == Camera()->GetProjType())
		MovePerspective();
	else
		MoveOrthographic();
}

void CCamMoveScript::MovePerspective() {
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();

	Vec3 vFront = Transform()->GetDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetDir(DIR::RIGHT);


	if (1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos += vFront * 10.f;
	if (-1 == KeyMgr::GetInst()->GetMouseWheel())
		vPos -= vFront * 10.f;


	if (KEY_PRESSED(KEY::W))
		vPos += vFront * 500.f * E_DT;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * 500.f * E_DT;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * 500.f * E_DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * 500.f * E_DT;

	if (KEY_PRESSED(KEY::RBTN))	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		vRot.y += vMouseDir.x * E_DT * XM_2PI * 3.f;
		vRot.x += vMouseDir.y * E_DT * XM_2PI * 3.f;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);
}

void CCamMoveScript::MoveOrthographic() {
	Vec3 vPos = Transform()->GetRelativePos();	

	if (KEY_PRESSED(KEY::W))
		EditorMgr::GetInst()->SetGizmoState(EGizmoState::E_Trans);
	else if (KEY_PRESSED(KEY::E))
		EditorMgr::GetInst()->SetGizmoState(EGizmoState::E_Rotate);
	else if (KEY_PRESSED(KEY::R)) 
		EditorMgr::GetInst()->SetGizmoState(EGizmoState::E_Scale);

	if (EditorMgr::GetInst()->IsMouseOnScene()) {
		if (KEY_TAP(KEY::RBTN)) {
			auto mpos = KeyMgr::GetInst()->GetMousePosOnScene();

			m_MousePrevPos = mpos;
		}
		else if (KEY_PRESSED(KEY::RBTN)) {
			auto editor = EditorMgr::GetInst()->FindUI("Scene");
			auto scene = static_cast<SceneUI*>(editor.Get());
			auto screenSize = scene->GetSceneSize();

			auto mpos = KeyMgr::GetInst()->GetMousePosOnScene();

			auto wpos = RenderMgr::GetInst()->GetEditorCamera()->Camera()->ScreenToWorld(
				mpos, screenSize);
			auto prevWPos = RenderMgr::GetInst()->GetEditorCamera()->Camera()->ScreenToWorld(
				m_MousePrevPos, screenSize);

			auto delta = prevWPos - wpos;
			vPos += delta;

			m_MousePrevPos = mpos;
		}

		auto wheel = KeyMgr::GetInst()->GetMouseWheel();
		if (wheel < 0) {
			auto scale = Camera()->GetOrthoScale();
			scale += 0.05f;
			Camera()->SetOrthoScale(scale);
		}
		else if (wheel > 0) {
			auto scale = Camera()->GetOrthoScale();
			scale -= 0.05f;
			scale = max(scale, 1e-6f);
			Camera()->SetOrthoScale(scale);
		}
	}	

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
}
