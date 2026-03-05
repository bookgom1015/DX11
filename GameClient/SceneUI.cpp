#include "pch.h"
#include "SceneUI.h"

#include "Device.h"
#include "LevelMgr.h"
#include "KeyMgr.h"
#include "RenderMgr.h"
#include "EditorMgr.h"
#include "CollisionMgr.h"

#include "Inspector.h"

namespace {
    struct GizmoState {
        EGizmoAxis::Type hot = EGizmoAxis::None;    // hover 중인 축
        EGizmoAxis::Type active = EGizmoAxis::None; // 드래그 중인 축

        ImVec2 dragStartMouseAbs{};         // absolute mouse pos (픽셀)
        Vec3   dragStartWorld{};            // 오브젝트 시작 월드
    };

    static GizmoState g;

    bool HitCircle(const ImVec2& p, const ImVec2& c, float r) {
        ImVec2 d = ImVec2(p.x - c.x, p.y - c.y);
        return (d.x * d.x + d.y * d.y) <= r * r;
    }

    bool HitLine(const ImVec2& p, const ImVec2& a, const ImVec2& b, float thickness) {
        ImVec2 ab = ImVec2(b.x - a.x, b.y - a.y);
        ImVec2 ap = ImVec2(p.x - a.x, p.y - a.y);

        float abLenSq = ab.x * ab.x + ab.y * ab.y;
        if (abLenSq <= 0.000001f)
            return false;

        float t = (ap.x * ab.x + ap.y * ab.y) / abLenSq;
        t = min(max(t, 0.0f), 1.0f);

        ImVec2 closest = ImVec2(a.x + ab.x * t, a.y + ab.y * t);

        ImVec2 d = ImVec2(p.x - closest.x, p.y - closest.y);
        float distSq = d.x * d.x + d.y * d.y;

        float r = thickness * 0.5f;
        return distSq <= r * r;
    }
}

SceneUI::SceneUI() : EditorUI("Scene") {}

SceneUI::~SceneUI() {}

void SceneUI::Tick_UI() {
	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	m_SceneHWND = (HWND)viewport->PlatformHandle;

	{
		float windowWidth = ImGui::GetContentRegionAvail().x;
		float childWidth = 200.0f;

		ImGui::SetCursorPosX((windowWidth - childWidth) * 0.5f);
		ImGui::BeginChild("Buttons", Vec2(childWidth, 36.f));
		
		if (ImGui::Button("Play", Vec2(48.f, 32.f))) 
			ChangeLevelState(ELevelState::E_Playing);
		ImGui::SameLine();

		if (ImGui::Button("Pause", Vec2(48.f, 32.f))) 
			ChangeLevelState(ELevelState::E_Paused);
		ImGui::SameLine();

		if (ImGui::Button("Stop", Vec2(48.f, 32.f))) 
			ChangeLevelState(ELevelState::E_Stopped);

		ImGui::EndChild();
	}

	ImGui::Separator();

	ImVec2 avail = ImGui::GetContentRegionAvail();   // 지금 커서 위치에서 남은 영역
	if (avail.x < 1.f) avail.x = 1.f;
	if (avail.y < 1.f) avail.y = 1.f;

	m_SceneSize = Vec2(avail.x, avail.y);

	{		
		auto srv = Device::GetInst()->GetScene_Srv();
		ImGui::Image((ImTextureID)srv, avail);

		// 씬 이미지의 absolute 좌상단(스크린 좌표) 저장
		m_SceneMin = ImGui::GetItemRectMin();;
		m_SceneMax = ImGui::GetItemRectMax();;

        m_SceneHovered = ImGui::IsItemHovered();

		// 방금 그린 위젯 기준
		if (ImGui::IsItemHovered()) {
			ImVec2 mouseScreen = ImGui::GetMousePos();
			ImVec2 itemMin = ImGui::GetItemRectMin();

			ImVec2 localPos;
			localPos.x = mouseScreen.x - itemMin.x;
			localPos.y = mouseScreen.y - itemMin.y;

			auto vec = KeyMgr::GetInst()->GetMousePos();

			KeyMgr::GetInst()->SetMousePosOnScene({ localPos.x, localPos.y });

			//ImGui::Text("Local Mouse: %.1f, %.1f", localPos.x, localPos.y);
			//ImGui::SameLine();
			//ImGui::Text("Mouse: %.1f, %.1f", vec.x, vec.y);
		}
	}

    PickUpTarget();
	Draw2DGizmo();
}

void SceneUI::PickUpTarget() {
    if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing 
        || !ImGui::IsMouseClicked(0)) return;

    ImGuiIO& io = ImGui::GetIO();
    // 씬 영역 밖이면 입력 무시 (렌더는 클립으로 잘릴 수 있지만 입력은 따로 막는 게 좋음)
    bool mouseInScene =
        (io.MousePos.x >= m_SceneMin.x && io.MousePos.x < m_SceneMax.x &&
            io.MousePos.y >= m_SceneMin.y && io.MousePos.y < m_SceneMax.y);
    if (!mouseInScene) return;

    auto mpos = KeyMgr::GetInst()->GetMousePosOnScene();
    auto wpos = RenderMgr::GetInst()->GetEditorCamera()->Camera()->ScreenToWorld(
        mpos, m_SceneSize);

    //EditorMgr::GetInst()->AddInfoLog(std::format("({}, {}, {})", wpos.x, wpos.y, wpos.z));

    for (int i = 0; i < ELevelLayer::Count; ++i) {
        auto level =LevelMgr::GetInst()->GetCurLevel()->GetLayer(i);
        auto objects = level->GetAllObjects();

        for (const auto& object : objects) {
            auto collider = object->Collider2D();
            if (collider == nullptr) continue;

            if (CollisionMgr::GetInst()->IsCollision(collider, wpos)) {
                std::string name{};
                name.assign(object->GetName().begin(), object->GetName().end());

                EditorMgr::GetInst()->AddInfoLog(
                    std::format("{} 선택", name));

                auto inspector = static_cast<Inspector*>(EditorMgr::GetInst()->FindUI("Inspector").Get());
                inspector->SetTargetObject(object);

                break;
            }
        }
    }
}

void SceneUI::Draw2DGizmo() {
    if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing) return;

    auto inspector = static_cast<Inspector*>(EditorMgr::GetInst()->FindUI("Inspector").Get());
    auto target = inspector->GetTargetObject();
    if (target == nullptr) return;

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // 씬 영역 밖이면 입력 무시 (렌더는 클립으로 잘릴 수 있지만 입력은 따로 막는 게 좋음)
    bool mouseInScene =
        (io.MousePos.x >= m_SceneMin.x && io.MousePos.x < m_SceneMax.x &&
            io.MousePos.y >= m_SceneMin.y && io.MousePos.y < m_SceneMax.y);

    bool allowGizmoInput = mouseInScene && m_SceneHovered;

    // --- 위치 계산 (absolute) ---
    Vec3 wpos = target->Transform()->GetRelativePos();
    ImVec2 local = RenderMgr::GetInst()->GetEditorCamera()->Camera()->WorldToScreen(wpos, m_SceneSize);
    ImVec2 screenPos = ImVec2(m_SceneMin.x + local.x, m_SceneMin.y + local.y);

    const float axisLen = 60.f;
    const float pickR = 12.f;  // 클릭 판정 반경
    const float handleR = 10.f;
    const float thickness = 4.f;

    ImVec2 xEnd = ImVec2(screenPos.x + axisLen, screenPos.y);
    ImVec2 yEnd = ImVec2(screenPos.x, screenPos.y - axisLen);

    // --- 클리핑 ---
    dl->PushClipRect(m_SceneMin, m_SceneMax, true);

    // --- Hover(Hot) 판정 ---
    g.hot = EGizmoAxis::None;
    if (allowGizmoInput && g.active == EGizmoAxis::None) {
        if (HitCircle(io.MousePos, screenPos, pickR)) g.hot = EGizmoAxis::MoveXY;
        else if (HitLine(io.MousePos, screenPos, xEnd, thickness)) g.hot = EGizmoAxis::MoveX;
        else if (HitCircle(io.MousePos, xEnd, pickR)) g.hot = EGizmoAxis::MoveX;
        else if (HitLine(io.MousePos, screenPos, yEnd, thickness)) g.hot = EGizmoAxis::MoveY;
        else if (HitCircle(io.MousePos, yEnd, pickR)) g.hot = EGizmoAxis::MoveY;        
    }                                                         

    // --- 클릭 시작 ---
    if (allowGizmoInput
        && ImGui::IsMouseClicked(0) 
        && g.hot != EGizmoAxis::None) {
        g.active = g.hot;
        g.dragStartMouseAbs = io.MousePos;
        g.dragStartWorld = wpos;

        // 여기서 "클릭했을 때 실행" (Begin 이벤트)
        OnGizmoBegin(g.active, target);
    }

    // --- 드래그 중 ---
    if (g.active != EGizmoAxis::None && ImGui::IsMouseDown(0)) {
        // 마우스 이동량을 월드 이동량으로 변환하는 가장 쉬운 방법:
        // ScreenToWorld(현재) - ScreenToWorld(시작)
        // ※ ScreenToWorld는 "scene 로컬" 좌표를 넣어야 함!
        Vec2 startLocal = Vec2(g.dragStartMouseAbs.x - m_SceneMin.x, g.dragStartMouseAbs.y - m_SceneMin.y);
        Vec2 currLocal = Vec2(io.MousePos.x - m_SceneMin.x, io.MousePos.y - m_SceneMin.y);

        Vec3 wStart = RenderMgr::GetInst()->GetEditorCamera()->Camera()->ScreenToWorld(startLocal, m_SceneSize);
        Vec3 wCurr = RenderMgr::GetInst()->GetEditorCamera()->Camera()->ScreenToWorld(currLocal, m_SceneSize);

        Vec3 delta = wCurr - wStart;

        Vec3 newPos = g.dragStartWorld;

        if (g.active == EGizmoAxis::MoveXY) { newPos += delta; }
        else if (g.active == EGizmoAxis::MoveX) { newPos.x += delta.x; }
        else if (g.active == EGizmoAxis::MoveY) { newPos.y += delta.y; }

        target->Transform()->SetRelativePos(newPos);

        // 드래그 중 실행
        OnGizmoDrag(g.active, target, newPos);
    }

    // --- 드래그 종료 ---
    if (g.active != EGizmoAxis::None && ImGui::IsMouseReleased(0)) {
        // 드래그 끝 실행
        OnGizmoEnd(g.active, target);

        g.active = EGizmoAxis::None;
    }

    // --- 렌더 (hot/active에 따라 색 바꾸면 더 좋음) ---
    auto colX = IM_COL32(255, 0, 0, 255);
    auto colY = IM_COL32(0, 255, 0, 255);
    auto colC = IM_COL32(200, 200, 0, 255);

    // active면 강조(예: 노랑) 같은 식으로도 가능
    if (g.active == EGizmoAxis::MoveX || g.hot == EGizmoAxis::MoveX) 
        colX = IM_COL32(255, 255, 0, 255);
    if (g.active == EGizmoAxis::MoveY || g.hot == EGizmoAxis::MoveY) 
        colY = IM_COL32(255, 255, 0, 255);
    if (g.active == EGizmoAxis::MoveXY || g.hot == EGizmoAxis::MoveXY) 
        colC = IM_COL32(255, 255, 0, 255);

    // Gizmo X-Axis
    dl->AddLine(screenPos, xEnd, colX, thickness);
    dl->AddCircleFilled(xEnd, handleR, colX);
    // Gizmo Y-Axis
    dl->AddLine(screenPos, yEnd, colY, thickness);
    dl->AddCircleFilled(yEnd, handleR, colY);
    // Center
    dl->AddCircleFilled(screenPos, handleR, colC);

    dl->PopClipRect();
}

void SceneUI::OnGizmoBegin(EGizmoAxis::Type axis, Ptr<GameObject> target) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 시작");
}

void SceneUI::OnGizmoDrag(EGizmoAxis::Type axis, Ptr<GameObject> target, Vec3 pos) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 중");
}

void SceneUI::OnGizmoEnd(EGizmoAxis::Type axis, Ptr<GameObject> target) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 끝");
}