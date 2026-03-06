#ifndef __ATILEMAP_INL__
#define __ATILEMAP_INL__

UINT ATileMap::GetRow() const { return m_Row; }
UINT ATileMap::GetCol() const { return m_Col; }

const vector<Ptr<ASprite>>& ATileMap::GetSprites() const { return m_vecSpriteInfo; }

#endif // __ATILEMAP_INL__