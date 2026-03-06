#pragma once

#include "ComponentUI.h"

class Collider2DUI : public ComponentUI {
public:
    Collider2DUI();
    virtual ~Collider2DUI();

public:
    virtual void Tick_UI() override;
};

