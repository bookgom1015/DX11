#pragma once

#include "AssetUI.h"

class SoundUI : public AssetUI {
public:
    SoundUI();
    virtual ~SoundUI();

public:
    virtual void Tick_UI() override;
};

