#pragma once

#include "CRenderComponent.h"

#include "ATileMap.h"
#include "StructuredBuffer.h"

struct SpriteInfo {
    Vec2 LeftTop;
    Vec2 Slice;
};

class CTileRender : public CRenderComponent {
public:
    CTileRender();
    virtual ~CTileRender();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;

public:
    void SetTileMap(Ptr<ATileMap> _TileMap);

private:
    Ptr<ATileMap>           m_TileMap;
    vector<SpriteInfo>      m_vecSpriteInfo;
    Ptr<StructuredBuffer>   m_Buffer;
};

