#pragma once
#include "EditorUI.h"

class Menu :
    public EditorUI
{
private:
    void File();
    void View();
    void GameObject();
    void Asset();
    void Render();

public:
    virtual void Tick() override;
    virtual void Tick_UI() override;

private:
    wstring GetAssetName(ASSET_TYPE _Type, const wstring& _Name);

    void CloseButton();

public:
    Menu();
    virtual ~Menu();
};

