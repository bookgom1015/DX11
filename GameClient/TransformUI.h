#pragma once

#include "ComponentUI.h"

class TransformUI : public ComponentUI {
public:
    TransformUI();
    virtual ~TransformUI();

public:
    virtual void Tick_UI() override;
};

