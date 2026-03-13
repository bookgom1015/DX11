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
    CTileRender(const CTileRender& _Origin);
    virtual ~CTileRender();

public:
    virtual void Init() override;
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;

public:
    CLONE(CTileRender);

    virtual void SaveToLevelFile(FILE* const _FileStream) override;
    virtual void LoadFromLevelFile(FILE* const _FileStream) override;

public:
    void SetTileMap(Ptr<ATileMap> _TileMap);
    Ptr<ATileMap> GetTileMap() const { return m_TileMap; }

private:
    Ptr<ATileMap> m_TileMap;
    vector<SpriteInfo> m_vecSpriteInfo;
    Ptr<StructuredBuffer> m_Buffer;
};

