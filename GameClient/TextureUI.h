#pragma once

#include "AssetUI.h"

class TextureUI : public AssetUI {
public:
    TextureUI();
    virtual ~TextureUI();

public:
    virtual void Tick_UI() override;
};

