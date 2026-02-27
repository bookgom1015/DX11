#pragma once
#include "AssetUI.h"
class MaterialUI :
    public AssetUI
{
public:
    virtual void Tick_UI() override;

private:
    void SelectShader(DWORD_PTR _ListUI);

public:
    MaterialUI();
    virtual ~MaterialUI();
};