#pragma once
#include "EditorUI.h"

class Profiler : public EditorUI {
public:
    Profiler();
    virtual ~Profiler();

public:
    virtual void Tick_UI() override;

private:
    void FrameRateText();

private:
    FLOAT mFrameTimes[3000]{};
    UINT mFrameOffset{};
};


