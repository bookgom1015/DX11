#pragma once

#include "Asset.h"

#include "ASprite.h"

class AFlipbook : public Asset {
public:
    AFlipbook();
    virtual ~AFlipbook();

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    __forceinline void AddSprite(Ptr<ASprite> _Sprite);
    __forceinline void SetSprite(int _Idx, Ptr<ASprite> _Sprite);

    __forceinline Ptr<ASprite> GetSprite(int _Idx) const;
    __forceinline UINT GetSpriteCount() const;

private:
    vector<Ptr<ASprite>>    m_vecSprite;
};

#include "AFlipbook.inl"