#pragma once

#include "ATexture.h"
#include "ASprite.h"

#include "AssetUI.h"

class FlipbookEditorUI : public EditorUI {
public:
    FlipbookEditorUI();
    virtual ~FlipbookEditorUI();

public:
    virtual void Tick_UI() override;

private:
    void LeftPanel();
    void RightPanel();
    void LowerPanel();

    void MakeSprites();
    void SaveSprites();

private:
    Ptr<ATexture> m_Texture;
    vector<Ptr<ASprite>> m_Sprites;

    int m_Grid[2];

    Vec2 m_BackgroundUV;
};