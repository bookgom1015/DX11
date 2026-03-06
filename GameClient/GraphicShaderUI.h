#pragma once

#include "AssetUI.h"

class GraphicShaderUI : public AssetUI {
public:
    GraphicShaderUI();
    virtual ~GraphicShaderUI();

public:
    virtual void Tick_UI() override;
};

