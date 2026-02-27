#include "pch.h"
#include "ATileMap.h"




ATileMap::ATileMap()
	: Asset(ASSET_TYPE::TILEMAP)
	, m_Row(0)
	, m_Col(0)
{
}

ATileMap::~ATileMap()
{
}

void ATileMap::SetRowCol(UINT _Row, UINT _Col)
{
	m_Row = _Row;
	m_Col = _Col;
	m_vecSpriteInfo.resize(m_Row * m_Col);
}

void ATileMap::SetSprite(UINT _Row, UINT _Col, Ptr<ASprite> _Sprite)
{
	if (nullptr == m_Atlas || _Sprite->GetAtlas() != m_Atlas)
		return;

	// 2 차원 행렬 좌표를 1차원 인덱스로 변환
	int Idx = _Row * m_Col + _Col;
	m_vecSpriteInfo[Idx] = _Sprite;
}

int ATileMap::Load(const wstring& _FilePath)
{
	return 0;
}

int ATileMap::Save(const wstring& _FilePath)
{
	return 0;
}