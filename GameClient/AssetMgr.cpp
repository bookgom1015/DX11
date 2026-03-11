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
	auto iter = m_mapAsset[_Asset->GetType()].find(_Key);
	auto end = m_mapAsset[_Asset->GetType()].end();
	// 동일한 Key가 이미 있어도 경로가 같은 파일이면 동일 파일로 인식하여 무시
	assert(iter == end || (iter->second->GetRelativePath() == _Asset->GetRelativePath()));
	if (iter != end) return;

	_Asset->SetKey(_Key);
	m_mapAsset[(UINT)_Asset->GetType()].insert(make_pair(_Key, _Asset));

	m_Changed = true;
}

void AssetMgr::GetAssetNames(EAsset::Type _type, vector<wstring>& _vec) {
	for (const auto& pair : m_mapAsset[(UINT)_type])
		_vec.push_back(pair.first);
}
