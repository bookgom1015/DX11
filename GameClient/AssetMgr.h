#pragma once

#include "assets.h"
#include "PathMgr.h"
#include "ALevel.h"

class AssetMgr : public singleton<AssetMgr> {
	SINGLE(AssetMgr)

public:
	void Init();
	bool IsChanged();

private:
	void CreateEngineMesh();
	void CreateEngineShader();
	void CreateEngineTexture();
	void CreateEngineMaterial();
	void CreateEngineSprite();

public:
	void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);

	void GetAssetNames(EAsset::Type _type, vector<wstring>& _vec);

	Ptr<Asset> FindAsset(EAsset::Type _Type, const wstring& _Key);

	template<typename T>
	Ptr<T> Find(const wstring& _Key);

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

private:
	map<wstring, Ptr<Asset>> m_mapAsset[EAsset::Count];
	bool m_Changed;
};

#include "AssetMgr.inl"