#pragma once

#include "AssetUI.h"

class SceneUI : public EditorUI {
public:
    SceneUI();
    virtual ~SceneUI();

public:
    virtual void Tick_UI() override;

public:
    GET_SET(Vec2, SceneSize);
    GET(HWND, SceneHWND);

private:
    Vec2 m_SceneSize{};
    HWND m_SceneHWND{};
};

