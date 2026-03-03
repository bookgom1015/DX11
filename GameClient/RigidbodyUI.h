#pragma once

#include "ComponentUI.h"

class RigidbodyUI : public ComponentUI {
public:
    RigidbodyUI();
    virtual ~RigidbodyUI();

public:
    virtual void Tick_UI() override;
};

