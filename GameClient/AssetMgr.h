#pragma once

#include "assets.h"

class AssetMgr
	: public singleton<AssetMgr>
{
	SINGLE(AssetMgr)
private:
	map<wstring, Ptr<Asset>>	m_mapAsset[(UINT)ASSET_TYPE::END];

public:
	void Init();

private:
	void CreateEngineMesh();
	void CreateEngineShader();
	void CreateEngineTexture();
	void CreateEngineMaterial();

public:
	void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);

	Ptr<Asset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);

	template<typename T>
	Ptr<T> Find(const wstring& _Key);
};

template<typename T>
ASSET_TYPE GetAssetType()
{	
	if constexpr (std::is_same_v<T, AMesh>)
		return ASSET_TYPE::MESH;		
	else if constexpr (std::is_same_v<T, AMaterial>)
		return ASSET_TYPE::MATERIAL;
	else if constexpr (std::is_same_v<T, AGraphicShader>)
		return ASSET_TYPE::GRAPHICSHADER;
	else if constexpr (std::is_same_v<T, ATexture>)
		return ASSET_TYPE::TEXTURE;

	return ASSET_TYPE::END;
}

template<typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key)
{		
	ASSET_TYPE Type = GetAssetType<T>();

	map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter == m_mapAsset[(UINT)Type].end())
		return nullptr;

	return (T*)iter->second.Get();
}

#define FIND(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)