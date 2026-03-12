#pragma once

#include "assets.h"
#include "PathMgr.h"
#include "ALevel.h"

class AssetMgr : public singleton<AssetMgr> {
	SINGLE(AssetMgr)

public:
	void Init();
	bool IsChanged();

	void Tick();

private:
	void CreateEngineMesh();
	void CreateEngineShader();
	void CreateEngineTexture();
	void CreateEngineMaterial();

	void LoadTextures();
	void LoadSprites();
	void LoadLevels();

	void LoadAssets(
		const wstring& folder, 
		const unordered_set<string>& extensions,
		const std::function<void(const wstring&)>& func);

	void WatchDirectory(const std::wstring& folderPath);

public:
	void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);

	void GetAssetNames(EAsset::Type _type, vector<wstring>& _vec);

	Ptr<Asset> FindAsset(EAsset::Type _Type, const wstring& _Key);

	template<typename T>
	Ptr<T> Find(const wstring& _Key);

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	template<typename T>
	Ptr<T> LoadForcefully(const wstring& _Key, const wstring& _RelativePath);

private:
	map<wstring, Ptr<Asset>> m_mapAsset[EAsset::Count];
	bool m_Changed;

	thread m_WatcherThread;
	mutex m_Mutex;
	bool m_Quit;

	vector<wstring> m_Logs;
	vector<pair<chrono::steady_clock::time_point, wstring>> m_Files;

	HANDLE m_DirHandle;

	long long m_Delay;
};

#include "AssetMgr.inl"