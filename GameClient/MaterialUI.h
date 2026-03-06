#pragma once

#include "AssetUI.h"

class MaterialUI : public AssetUI {
public:
    MaterialUI();
    virtual ~MaterialUI();

public:
    virtual void Tick_UI() override;

private:
    void ShaderParameter();
    void SelectShader(DWORD_PTR _ListUI);
};