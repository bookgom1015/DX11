#include "pch.h"
#include "AssetMgr.h"

#include "PathMgr.h"

#include "AMesh.h"
#include "AGraphicShader.h"
#include "ATexture.h"

AssetMgr::AssetMgr() : m_Changed(false) {}

AssetMgr::~AssetMgr() {}

bool AssetMgr::IsChanged() {
	bool Changed = m_Changed;
	m_Changed = false;

	return Changed;
}

Ptr<Asset> AssetMgr::FindAsset(EAsset::Type _Type, const wstring& _Key) {
	auto iter = m_mapAsset[(UINT)_Type].find(_Key);
	if (iter == m_mapAsset[(UINT)_Type].end()) return nullptr;

	return iter->second;
}

void AssetMgr::AddAsset(const wstring& _Key, Ptr<Asset> _Asset) {	
	// 동일한 Key 로 등록된 적이 없어야 한다.
	assert(m_mapAsset[(UINT)_Asset->GetType()].find(_Key) == m_mapAsset[(UINT)_Asset->GetType()].end());

	_Asset->SetKey(_Key);
	m_mapAsset[(UINT)_Asset->GetType()].insert(make_pair(_Key, _Asset));

	m_Changed = true;
}

void AssetMgr::GetAssetNames(EAsset::Type _type, vector<wstring>& _vec) {
	for (const auto& pair : m_mapAsset[(UINT)_type])
		_vec.push_back(pair.first);
}
