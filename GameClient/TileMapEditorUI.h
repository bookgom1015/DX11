#pragma once

#include "AssetUI.h"

#include "ATileMap.h"

class TileMapEditorUI : public EditorUI {
public:
    TileMapEditorUI();
    virtual ~TileMapEditorUI();

public:
    virtual void Tick_UI() override;

private:
    void LeftPanel();
    void Grid(Vec2 imageMin, Vec2 imageMax);
    void Sprite(Vec2 leftTopPos, Vec2 imageRectSize);
    void Selectable(Vec2 leftTopPos, Vec2 imageRectSize);

    void RightPanel();
    void GridControl();
    void BrushControl();
    void ControlButtons();

private:
    int m_BufGrid[2];
    int m_Grid[2];

    Ptr<ATileMap> m_TileMap;
    Ptr<ASprite> m_SpriteBrush;

    DragSelectState m_DragSelect;
    std::vector<SimpleRect> m_TileRects;
    std::vector<int> m_HoveredTiles;
};