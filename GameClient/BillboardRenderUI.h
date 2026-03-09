#pragma once

#include "ComponentUI.h"

class BillboardRenderUI : public ComponentUI {
public:
    BillboardRenderUI();
    virtual ~BillboardRenderUI();

public:
    virtual void Tick_UI() override;
};

