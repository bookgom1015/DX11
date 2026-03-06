#pragma once

#include "AssetUI.h"

class PrefabUI : public AssetUI {
public:
    PrefabUI();
    virtual ~PrefabUI();

public:
    virtual void Tick_UI() override;
};

