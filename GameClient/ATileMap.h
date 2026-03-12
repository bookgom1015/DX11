#pragma once

#include "Asset.h"
#include "ATexture.h"
#include "ASprite.h"

class ATileMap : public Asset {
public:
    ATileMap();
    virtual ~ATileMap();

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    GET_SET(Vec2, TileSize);
    GET_SET(Ptr<ATexture>, Atlas);

    void SetRowCol(UINT _Row, UINT _Col);    
    void SetSprite(UINT _Row, UINT _Col, Ptr<ASprite> _Sprite);

    Ptr<ASprite> GetSprite(UINT _Row, UINT _Col) {
        UINT index = _Row * m_Col + _Col;
        return m_vecSpriteInfo[index];
    }

    __forceinline UINT GetRow() const;
    __forceinline UINT GetCol() const;

    __forceinline const vector<Ptr<ASprite>>& GetSprites() const;

private:
    UINT m_Row;             // 타일맵의 행 개수
    UINT m_Col;             // 타일맵의 열 개수
    Vec2 m_TileSize;        // 타일맵을 구성하는 타일 1개의 크기

    Ptr<ATexture> m_Atlas;  // 타일맵을 구성하는 Sprite 들의 공통 아틀라스 텍스쳐
    vector<Ptr<ASprite>> m_vecSpriteInfo;
};

#include "ATileMap.inl"
