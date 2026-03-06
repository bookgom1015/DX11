#pragma once

#include "EditorUI.h"

class Menu : public EditorUI {
public:
    Menu();
    virtual ~Menu();

public:
    virtual void Tick() override;
    virtual void Tick_UI() override;

private:
    void File();
    void View();
    void GameObject();
    void Asset();
    void Render();

private:
    wstring GetAssetName(EAsset::Type _Type, const wstring& _Name);

    void CloseButton();
};

