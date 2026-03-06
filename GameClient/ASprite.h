#pragma once

#include "Asset.h"
#include "ATexture.h"

// 이미지 조각
class ASprite : public Asset {
public:
    ASprite();
    virtual ~ASprite();

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    GET_SET(Ptr<ATexture>, Atlas);
    GET_SET(Vec2, LeftTopUV);
    GET_SET(Vec2, SliceUV);
    GET_SET(Vec2, BackgroundUV);
    GET_SET(Vec2, OffsetUV);

private:
    Ptr<ATexture> m_Atlas;

    Vec2 m_LeftTopUV;
    Vec2 m_SliceUV;
    Vec2 m_BackgroundUV;
    Vec2 m_OffsetUV;
};

