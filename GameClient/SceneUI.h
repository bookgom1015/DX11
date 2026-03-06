#pragma once

#include "AssetUI.h"

class SceneUI : public EditorUI {
public:
    SceneUI();
    virtual ~SceneUI();

public:
    virtual void Tick_UI() override;

public:
    void SetGizmoState(EGizmoState::Type type) { m_GizmoState = type; }

private:
    void ControlButtons();
    void GizmoButtons();
    void Scene();

    void PickUpTarget();

    void DrawGizmo();
    void DrawTransGizmo(
        Ptr<GameObject> target, ImVec2 screenPos, bool allowGizmoInput);
    void DrawRotateGizmo(
        Ptr<GameObject> target, ImVec2 screenPos, bool allowGizmoInput);
    void DrawScaleGizmo(
        Ptr<GameObject> target, ImVec2 screenPos, bool allowGizmoInput);

private:
    void OnGizmoBegin(EGizmoAxis::Type axis, Ptr<GameObject> target);
    void OnGizmoDrag(EGizmoAxis::Type axis, Ptr<GameObject> target, Vec3 delta);
    void OnGizmoEnd(EGizmoAxis::Type axis, Ptr<GameObject> target);

public:
    GET_SET(Vec2, SceneSize);
    GET(HWND, SceneHWND);
    GET(bool, SceneHovered);

private:
    Vec2 m_SceneSize{};
    ImVec2 m_SceneMin{};
    ImVec2 m_SceneMax{};

    bool m_SceneHovered{};

    HWND m_SceneHWND{};

    EGizmoState::Type m_GizmoState;
};

