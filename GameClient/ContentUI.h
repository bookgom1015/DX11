#pragma once

#include "EditorUI.h"
#include "TreeUI.h"

class ContentUI : public EditorUI {
public:
    ContentUI();
    virtual ~ContentUI();

public:
    virtual void Tick_UI() override;

public:
    void Renew();

private:
    void SelectAsset(DWORD_PTR _Asset);

private:
    Ptr<TreeUI> m_Tree;
};

