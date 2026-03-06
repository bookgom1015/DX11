#pragma once

#include "ComponentUI.h"

class AddComponentButton : public ComponentUI {
public:
    AddComponentButton();
    virtual ~AddComponentButton();

public:
    virtual void Tick_UI() override;

public:
    void SelectScript(DWORD_PTR _ListUI);

private:
    void AddComponent();
    void AddScript();

private:
    float mLastButtonsWidth{};
};

