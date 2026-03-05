#pragma once

#include "ComponentUI.h"

class CameraUI : public ComponentUI {
public:
    CameraUI();
    virtual ~CameraUI();

public:
    virtual void Tick_UI() override;
};

