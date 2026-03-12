#pragma once

#include "ATexture.h"
#include "ASprite.h"

#include "AssetUI.h"

class AtlasEditorUI : public EditorUI {
public:
    AtlasEditorUI();
    virtual ~AtlasEditorUI();

public:
    virtual void Tick_UI() override;

private:
    void LeftPanel();
    void AtlasImage();
    void Grid(ImVec2 imageMin, ImVec2 imageMax);
    void BackgroundUV(ImVec2 imageMin, ImVec2 imageMax);
    void SelectableOnGrid(ImVec2 leftTopPos, ImVec2 imageRectSize);

    void RightPanel();
    void StatusInfo();
    void GridControl();
    void SpriteControl();
    void ControlButtons();

    void LowerPanel();

    void MakeSprites();
    void SaveSprites();

    bool IsTileSelected(int index);
    void SelectTile(int index);

    Vec2 GetSlice();

private:
    Ptr<ATexture> m_Texture;
    vector<pair<bool, Ptr<ASprite>>> m_Sprites;

    int m_Grid[2];

    Vec2 m_BackgroundUV;

    vector<int> m_SelectedTileIndices;
    
    string m_Prefix;
};