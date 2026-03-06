#pragma once

#include "AssetUI.h"

class LevelUI : public AssetUI {
public:
    LevelUI();
    virtual ~LevelUI();

public:
    virtual void Tick_UI() override;
};

