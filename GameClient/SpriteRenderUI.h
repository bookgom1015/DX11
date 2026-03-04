#pragma once

#include "ComponentUI.h"

class SpriteRenderUI : public ComponentUI {
public:
    SpriteRenderUI();
    virtual ~SpriteRenderUI();

public:
    virtual void Tick_UI() override;
};

