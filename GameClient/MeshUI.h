#pragma once

#include "AssetUI.h"

class MeshUI : public AssetUI {
public:
    MeshUI();
    virtual ~MeshUI();

public:
    virtual void Tick_UI() override;
};

