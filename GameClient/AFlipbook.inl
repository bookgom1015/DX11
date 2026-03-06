#ifndef __AFLIPBOOK_INL__
#define __AFLIPBOOK_INL__

void AFlipbook::AddSprite(Ptr<ASprite> _Sprite) { m_vecSprite.push_back(_Sprite); }

void AFlipbook::SetSprite(int _Idx, Ptr<ASprite> _Sprite) {
    if (m_vecSprite.size() <= _Idx)
        m_vecSprite.resize(_Idx + 1);

    m_vecSprite[_Idx] = _Sprite;
}

Ptr<ASprite> AFlipbook::GetSprite(int _Idx) const { return m_vecSprite[_Idx]; }

UINT AFlipbook::GetSpriteCount() const { return static_cast<UINT>(m_vecSprite.size()); }

#endif // __AFLIPBOOK_INL__