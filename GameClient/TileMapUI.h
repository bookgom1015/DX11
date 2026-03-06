#pragma once

#include "AssetUI.h"

class TileMapUI : public AssetUI {
public:
    TileMapUI();
    virtual ~TileMapUI();

public:
    virtual void Tick_UI() override;
};

