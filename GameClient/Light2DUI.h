#pragma once

#include "ComponentUI.h"

class Light2DUI : public ComponentUI {
public:
    Light2DUI();
    virtual ~Light2DUI();

public:
    virtual void Tick_UI() override;
};

