#pragma once

#include "ComponentUI.h"

class TileRenderUI : public ComponentUI {
public:
    TileRenderUI();
    virtual ~TileRenderUI();

public:
    virtual void Tick_UI() override;
};

