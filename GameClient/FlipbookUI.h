#pragma once

#include "AssetUI.h"

class FlipbookUI : public AssetUI {
public:
    FlipbookUI();
    virtual ~FlipbookUI();

public:
    virtual void Tick_UI() override;

private:
    float mElapsedTime;
    float mFPS;
    UINT mIndex;
};

