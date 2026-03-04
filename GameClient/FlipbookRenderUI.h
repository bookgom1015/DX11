#pragma once

#include "ComponentUI.h"

class FlipbookRenderUI : public ComponentUI {
public:
    FlipbookRenderUI();
    virtual ~FlipbookRenderUI();

public:
    virtual void Tick_UI() override;
};

