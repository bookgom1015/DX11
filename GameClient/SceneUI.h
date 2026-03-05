#pragma once

#include "AssetUI.h"

class SceneUI : public EditorUI {
public:
    SceneUI();
    virtual ~SceneUI();

public:
    virtual void Tick_UI() override;

public:
    void PickUpTarget();
    void Draw2DGizmo();

private:
    void OnGizmoBegin(EGizmoAxis::Type axis, Ptr<GameObject> target);
    void OnGizmoDrag(EGizmoAxis::Type axis, Ptr<GameObject> target, Vec3 pos);
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
};

