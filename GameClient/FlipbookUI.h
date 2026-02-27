#pragma once
#include "AssetUI.h"
class FlipbookUI :
    public AssetUI
{
public:
    virtual void Tick_UI() override;

public:
    FlipbookUI();
    virtual ~FlipbookUI();

private:
    float mElapsedTime{};
    float mFPS{ 15.f };
    UINT mIndex{};
};

