#pragma once

#include "AssetUI.h"

class SpriteUI : public AssetUI {
public:
    SpriteUI();
    virtual ~SpriteUI();

public:
    virtual void Tick_UI() override;
};

