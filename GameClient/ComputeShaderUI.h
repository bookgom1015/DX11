#pragma once

#include "AssetUI.h"

class ComputeShaderUI : public AssetUI {
public:
    ComputeShaderUI();
    virtual ~ComputeShaderUI();

public:
    virtual void Tick_UI() override;
};

