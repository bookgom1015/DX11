#include "pch.h"
#include "SceneUI.h"

#include "Device.h"
#include "LevelMgr.h"
#include "KeyMgr.h"
#include "RenderMgr.h"
#include "EditorMgr.h"
#include "CollisionMgr.h"
#include "TimeMgr.h"

#include "Inspector.h"

namespace {
    struct GizmoState {
        EGizmoAxis::Type hot = EGizmoAxis::None;    // hover 중인 축
        EGizmoAxis::Type active = EGizmoAxis::None; // 드래그 중인 축

        ImVec2 dragStartMouseAbs{};         // absolute mouse pos (픽셀)
        Vec3   dragStartWorld{};            // 오브젝트 시작 월드
        Vec3   dragStartScale{};            // 오브젝트 시작 크기
        Vec3   dragStartRotate{};
        float  dragStartAngle{};
    };

    static GizmoState g;

    bool HitCircle(const ImVec2& p, const ImVec2& c, float r) {
        ImVec2 d = ImVec2(p.x - c.x, p.y - c.y);
        return (d.x * d.x + d.y * d.y) <= r * r;
    }

    bool HitRing(const ImVec2& p, const ImVec2& c, float radius, float thickness) {
        const float dx = p.x - c.x;
        const float dy = p.y - c.y;
        const float distSq = dx * dx + dy * dy;

        const float outer = radius + thickness * 0.5f;
        const float inner = radius - thickness * 0.5f;

        return distSq <= outer * outer && distSq >= inner * inner;
    }

    bool HitRect(const ImVec2& p, const ImVec2& min, const ImVec2& max) {
        return (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y);
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

    float NormalizeAngleRad(float a) {
        while (a > XM_PI)  a -= XM_PI * 2.f;
        while (a < -XM_PI) a += XM_PI * 2.f;
        return a;
    }

    const float axisLen = 60.f;
    const float pickR = 12.f;  // 클릭 판정 반경
    const float handleR = 10.f;
    const float thickness = 4.f;
    const float ringThickness = 16.f;
}

SceneUI::SceneUI() : EditorUI("Scene")
    , m_GizmoState{ EGizmoState::E_Trans } {}

SceneUI::~SceneUI() {}

void SceneUI::Tick_UI() {
	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	m_SceneHWND = (HWND)viewport->PlatformHandle;

    ControlButtons();
    GizmoButtons();    	
    Scene();

    PickUpTarget();
    
    DrawGizmo();
}

void SceneUI::ControlButtons() {
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float childWidth = 200.0f;

    ImGui::SetCursorPosX((windowWidth - childWidth) * 0.5f);
    ImGui::BeginChild("Buttons", Vec2(childWidth, 36.f));

    auto state = LevelMgr::GetInst()->GetLevelState();

    bool playing = state == ELevelState::E_Playing;
    if (playing) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("Play", Vec2(48.f, 32.f)))
        ChangeLevelState(ELevelState::E_Playing);
    if (playing) ImGui::PopStyleColor();
    ImGui::SameLine();

    bool paused = state == ELevelState::E_Paused;
    if (paused) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("Pause", Vec2(48.f, 32.f)))
        ChangeLevelState(ELevelState::E_Paused);
    if (paused) ImGui::PopStyleColor();
    ImGui::SameLine();

    bool stopped = state == ELevelState::E_Stopped;
    if (stopped) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("Stop", Vec2(48.f, 32.f)))
        ChangeLevelState(ELevelState::E_Stopped);
    if (stopped) ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::Separator();
}

void SceneUI::GizmoButtons() {
    ImGui::BeginChild("GizmoButtons", Vec2(40.f, 0.f));

    float windowWidth = ImGui::GetContentRegionAvail().x;
    float buttonWidth = 35.0f;
    float margin = 5.f;

    ImGui::Dummy(Vec2(0.f, margin));

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    bool trans = m_GizmoState == EGizmoState::E_Trans;
    if (trans) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("T", Vec2(buttonWidth))) m_GizmoState = EGizmoState::E_Trans;
    ImGui::Dummy(Vec2(0.f, margin));
    if (trans) ImGui::PopStyleColor();

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    bool rotate = m_GizmoState == EGizmoState::E_Rotate;
    if (rotate) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("R", Vec2(buttonWidth))) m_GizmoState = EGizmoState::E_Rotate;
    ImGui::Dummy(Vec2(0.f, margin));
    if (rotate) ImGui::PopStyleColor();

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    bool scale = m_GizmoState == EGizmoState::E_Scale;
    if (scale) ImGui::PushStyleColor(
        ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button("S", Vec2(buttonWidth))) m_GizmoState = EGizmoState::E_Scale;
    if (scale) ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::SameLine();
}

void SceneUI::Scene() {
    ImVec2 avail = ImGui::GetContentRegionAvail();   // 지금 커서 위치에서 남은 영역
    if (avail.x < 1.f) avail.x = 1.f;
    if (avail.y < 1.f) avail.y = 1.f;

    m_SceneSize = Vec2(avail.x, avail.y);

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

void SceneUI::PickUpTarget() {
    if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing 
        || !ImGui::IsWindowFocused()
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

    for (int i = 0; i < ELevelLayer::Count; ++i) {
        auto level = LevelMgr::GetInst()->GetCurLevel()->GetLayer(i);
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

void SceneUI::DrawGizmo() {
    auto inspector = static_cast<Inspector*>(EditorMgr::GetInst()->FindUI("Inspector").Get());
    auto target = inspector->GetTargetObject();
    if (target == nullptr 
        || LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing) return;

    ImGuiIO& io = ImGui::GetIO();

    // 씬 영역 밖이면 입력 무시 (렌더는 클립으로 잘릴 수 있지만 입력은 따로 막는 게 좋음)
    bool mouseInScene =
        (io.MousePos.x >= m_SceneMin.x && io.MousePos.x < m_SceneMax.x &&
            io.MousePos.y >= m_SceneMin.y && io.MousePos.y < m_SceneMax.y);

    bool allowGizmoInput = mouseInScene && m_SceneHovered;

    // --- 위치 계산 (absolute) ---
    Vec3 wpos = target->Transform()->GetRelativePos();
    ImVec2 local = RenderMgr::GetInst()->GetEditorCamera()->Camera()->WorldToScreen(wpos, m_SceneSize);
    ImVec2 screenPos = ImVec2(m_SceneMin.x + local.x, m_SceneMin.y + local.y);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // --- 클리핑 ---
    dl->PushClipRect(m_SceneMin, m_SceneMax, true);

    if (m_GizmoState == EGizmoState::E_Trans) 
        DrawTransGizmo(target, screenPos, allowGizmoInput);
    else if (m_GizmoState == EGizmoState::E_Rotate)
        DrawRotateGizmo(target, screenPos, allowGizmoInput);
    else if (m_GizmoState == EGizmoState::E_Scale) 
        DrawScaleGizmo(target, screenPos, allowGizmoInput);

    dl->PopClipRect();
}

void SceneUI::DrawTransGizmo(
    Ptr<GameObject> target
    , ImVec2 screenPos
    , bool allowGizmoInput) {
    ImVec2 xEnd = ImVec2(screenPos.x + axisLen, screenPos.y);
    ImVec2 yEnd = ImVec2(screenPos.x, screenPos.y - axisLen);

    ImGuiIO& io = ImGui::GetIO();
    
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
        g.dragStartWorld = target->Transform()->GetRelativePos();
        g.dragStartRotate = target->Transform()->GetRelativeRot();

        ImVec2 v = ImVec2(io.MousePos.x - screenPos.x, io.MousePos.y - screenPos.y);
        g.dragStartAngle = atan2f(v.y, v.x);

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

        // 드래그 중 실행
        OnGizmoDrag(g.active, target, delta);
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

    ImDrawList* dl = ImGui::GetWindowDrawList();
    // Gizmo X-Axis
    dl->AddLine(screenPos, xEnd, colX, thickness);
    dl->AddCircleFilled(xEnd, handleR, colX);
    // Gizmo Y-Axis
    dl->AddLine(screenPos, yEnd, colY, thickness);
    dl->AddCircleFilled(yEnd, handleR, colY);
    // Center
    dl->AddCircleFilled(screenPos, handleR, colC);
}

void SceneUI::DrawRotateGizmo(
    Ptr<GameObject> target
    , ImVec2 screenPos
    , bool allowGizmoInput) {
    ImGuiIO& io = ImGui::GetIO();

    // --- Hover(Hot) 판정 ---
    g.hot = EGizmoAxis::None;
    if (allowGizmoInput && g.active == EGizmoAxis::None) {
        if (HitRing(io.MousePos, screenPos, axisLen, ringThickness))
            g.hot = EGizmoAxis::MoveXY;
    }    

    // --- 클릭 시작 ---
    if (allowGizmoInput
        && ImGui::IsMouseClicked(0)
        && g.hot != EGizmoAxis::None) {
        g.active = g.hot;
        g.dragStartMouseAbs = io.MousePos;
        g.dragStartRotate = target->Transform()->GetRelativeRot();

        ImVec2 v = ImVec2(io.MousePos.x - screenPos.x, io.MousePos.y - screenPos.y);
        g.dragStartAngle = atan2f(v.y, v.x);

        // 여기서 "클릭했을 때 실행" (Begin 이벤트)
        OnGizmoBegin(g.active, target);
    }

    float currAngle{};
    // --- 드래그 중 ---
    if (g.active != EGizmoAxis::None && ImGui::IsMouseDown(0)) {
        // 마우스 이동량을 월드 이동량으로 변환하는 가장 쉬운 방법:
        // ScreenToWorld(현재) - ScreenToWorld(시작)
        // ※ ScreenToWorld는 "scene 로컬" 좌표를 넣어야 함!
        Vec2 startLocal = Vec2(g.dragStartMouseAbs.x - m_SceneMin.x, g.dragStartMouseAbs.y - m_SceneMin.y);
        Vec2 currLocal = Vec2(io.MousePos.x - m_SceneMin.x, io.MousePos.y - m_SceneMin.y);

        ImVec2 v = ImVec2(io.MousePos.x - screenPos.x, io.MousePos.y - screenPos.y);

        // 중심에 너무 가까우면 atan2 불안정할 수 있으니 체크
        const float lenSq = v.x * v.x + v.y * v.y;
        float delta{};
        if (lenSq > 1.f) {
            currAngle = atan2f(v.y, v.x);
            delta = NormalizeAngleRad(g.dragStartAngle - currAngle);
        }

        // 드래그 중 실행
        OnGizmoDrag(g.active, target, Vec3(delta));
    }

    // --- 드래그 종료 ---
    if (g.active != EGizmoAxis::None && ImGui::IsMouseReleased(0)) {
        // 드래그 끝 실행
        OnGizmoEnd(g.active, target);

        g.active = EGizmoAxis::None;
    }

    auto colA = IM_COL32(255, 0, 0, 255);
    auto colB = IM_COL32(0, 255, 0, 255);
    auto colR = IM_COL32(200, 200, 0, 255);

    if (g.active == EGizmoAxis::MoveXY || g.hot == EGizmoAxis::MoveXY)
        colR = IM_COL32(255, 255, 0, 255);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddCircle(screenPos, axisLen, colR, 64, ringThickness);

    if (g.active != EGizmoAxis::None) {
        {
            float rad = g.dragStartAngle;
            auto sin = sinf(rad);
            auto cos = cosf(rad);

            Vec2 pos = Vec2(axisLen, 0.f);
            pos = Vec2(cos * pos.x - sin * pos.y, sin * pos.x + cos * pos.y);
            pos = pos + Vec2(screenPos.x, screenPos.y);

            dl->AddCircleFilled(pos, handleR, colA, 64);
        }
        {
            float rad = currAngle;
            auto sin = sinf(rad);
            auto cos = cosf(rad);

            Vec2 pos = Vec2(axisLen, 0.f);
            pos = Vec2(cos * pos.x - sin * pos.y, sin * pos.x + cos * pos.y);
            pos = pos + Vec2(screenPos.x, screenPos.y);

            dl->AddCircleFilled(pos, handleR, colB, 64);
        }
    }
}

void SceneUI::DrawScaleGizmo(
    Ptr<GameObject> target
    , ImVec2 screenPos
    , bool allowGizmoInput) {
    ImVec2 xEnd = ImVec2(screenPos.x + axisLen, screenPos.y);
    ImVec2 yEnd = ImVec2(screenPos.x, screenPos.y - axisLen);

    ImVec2 centerMin = ImVec2(screenPos.x - handleR, screenPos.y - handleR);
    ImVec2 centerMax = ImVec2(screenPos.x + handleR, screenPos.y + handleR);

    ImVec2 xMin = ImVec2(xEnd.x - handleR, xEnd.y - handleR);
    ImVec2 xMax = ImVec2(xEnd.x + handleR, xEnd.y + handleR);

    ImVec2 yMin = ImVec2(yEnd.x - handleR, yEnd.y - handleR);
    ImVec2 yMax = ImVec2(yEnd.x + handleR, yEnd.y + handleR);

    ImGuiIO& io = ImGui::GetIO();

    // --- Hover(Hot) 판정 ---
    g.hot = EGizmoAxis::None;
    if (allowGizmoInput && g.active == EGizmoAxis::None) {
        if (HitRect(io.MousePos, centerMin, centerMax)) g.hot = EGizmoAxis::MoveXY;
        else if (HitLine(io.MousePos, screenPos, xEnd, thickness)) g.hot = EGizmoAxis::MoveX;
        else if (HitRect(io.MousePos, xMin, xMax)) g.hot = EGizmoAxis::MoveX;
        else if (HitLine(io.MousePos, screenPos, yEnd, thickness)) g.hot = EGizmoAxis::MoveY;
        else if (HitRect(io.MousePos, yMin, yMax)) g.hot = EGizmoAxis::MoveY;
    }

    // --- 클릭 시작 ---
    if (allowGizmoInput
        && ImGui::IsMouseClicked(0)
        && g.hot != EGizmoAxis::None) {
        g.active = g.hot;
        g.dragStartMouseAbs = io.MousePos;
        g.dragStartScale = target->Transform()->GetRelativeScale();

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

        // 드래그 중 실행
        OnGizmoDrag(g.active, target, delta);
    }

    // --- 드래그 종료 ---
    if (g.active != EGizmoAxis::None && ImGui::IsMouseReleased(0)) {
        // 드래그 끝 실행
        OnGizmoEnd(g.active, target);

        g.active = EGizmoAxis::None;
    }

    auto colX = IM_COL32(255, 0, 0, 255);
    auto colY = IM_COL32(0, 255, 0, 255);
    auto colC = IM_COL32(200, 200, 0, 255);

    if (g.active == EGizmoAxis::MoveX || g.hot == EGizmoAxis::MoveX)
        colX = IM_COL32(255, 255, 0, 255);
    if (g.active == EGizmoAxis::MoveY || g.hot == EGizmoAxis::MoveY)
        colY = IM_COL32(255, 255, 0, 255);
    if (g.active == EGizmoAxis::MoveXY || g.hot == EGizmoAxis::MoveXY)
        colC = IM_COL32(255, 255, 0, 255);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    // Gizmo X-Axis
    dl->AddLine(screenPos, xEnd, colX, thickness);
    dl->AddRectFilled(xMin, xMax, colX);
    // Gizmo Y-Axis
    dl->AddLine(screenPos, yEnd, colY, thickness);
    dl->AddRectFilled(yMin, yMax, colY);
    // Center
    dl->AddRectFilled(centerMin, centerMax, colC);
}

void SceneUI::OnGizmoBegin(EGizmoAxis::Type axis, Ptr<GameObject> target) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 시작");
}

void SceneUI::OnGizmoDrag(EGizmoAxis::Type axis, Ptr<GameObject> target, Vec3 delta) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 중");

    if (m_GizmoState == EGizmoState::E_Trans) {
        Vec3 newPos = g.dragStartWorld;

        if (g.active == EGizmoAxis::MoveXY) { newPos += delta; }
        else if (g.active == EGizmoAxis::MoveX) { newPos.x += delta.x; }
        else if (g.active == EGizmoAxis::MoveY) { newPos.y += delta.y; }

        target->Transform()->SetRelativePos(newPos);
    }
    else if (m_GizmoState == EGizmoState::E_Rotate) {
        float newRot = g.dragStartRotate.z + delta.z;

        auto rot = target->Transform()->GetRelativeRot();

        target->Transform()->SetRelativeRot(Vec3(rot.x, rot.y, newRot));
    }
    else if (m_GizmoState == EGizmoState::E_Scale) {
        Vec3 newScale = g.dragStartScale;

        if (g.active == EGizmoAxis::MoveXY) { newScale += delta; }
        else if (g.active == EGizmoAxis::MoveX) { newScale.x += delta.x; }
        else if (g.active == EGizmoAxis::MoveY) { newScale.y += delta.y; }

        target->Transform()->SetRelativeScale(newScale);
    }
}

void SceneUI::OnGizmoEnd(EGizmoAxis::Type axis, Ptr<GameObject> target) {
    EditorMgr::GetInst()->AddInfoLog("기즈모 드래그 끝");
}