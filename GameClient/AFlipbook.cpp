#include "pch.h"
#include "AFlipbook.h"

#include "AssetMgr.h"

AFlipbook::AFlipbook() : Asset(EAsset::E_Flipbook) {}

AFlipbook::~AFlipbook() {}

int AFlipbook::Save(const wstring& _FilePath) {
	FILE* pFile{};
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	// 몇장의 스프라이트로 구성되었는지 저장
	UINT SpriteCount = (UINT)m_vecSprite.size();
	fwrite(&SpriteCount, sizeof(UINT), 1, pFile);

	// 가리키고 있었던 Sprite 가 누군지 저장
	for (const auto& Sprite : m_vecSprite)
		SaveAssetRef(pFile, Sprite.Get());

	fclose(pFile);	

	return 0;
}

int AFlipbook::Load(const wstring& _FilePath) {
	FILE* pFile{};
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	// 몇장의 스프라이트로 구성되었는지 저장
	UINT SpriteCount{};
	fread(&SpriteCount, sizeof(UINT), 1, pFile);

	// 가리키고 있었던 Sprite 가 누군지 저장
	for (UINT i = 0; i < SpriteCount; ++i) {
		Ptr<ASprite> pSprite = LoadAssetRef<ASprite>(pFile);

		AddSprite(pSprite);
	}

	fclose(pFile);

	return 0;
}
