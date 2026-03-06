#ifndef __ASSETMGR_INL__
#define __ASSETMGR_INL__

template<typename T>
EAsset::Type GetAssetType() {
	if constexpr (std::is_same_v<T, AMesh>)
		return EAsset::E_Mesh;
	else if constexpr (std::is_same_v<T, AMaterial>)
		return EAsset::E_Material;
	else if constexpr (std::is_same_v<T, AGraphicShader>)
		return EAsset::E_GraphicShader;
	else if constexpr (std::is_same_v<T, ATexture>)
		return EAsset::E_Texture;
	else if constexpr (std::is_same_v<T, ASprite>)
		return EAsset::E_Sprite;
	else if constexpr (std::is_same_v<T, AFlipbook>)
		return EAsset::E_Flipbook;
	else if constexpr (std::is_same_v<T, ATileMap>)
		return EAsset::E_TileMap;
	else if constexpr (std::is_same_v<T, ALevel>)
		return EAsset::E_Level;

	return EAsset::Count;
}

template<typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key) {
	EAsset::Type Type = GetAssetType<T>();

	auto iter = m_mapAsset[Type].find(_Key);
	if (iter == m_mapAsset[Type].end()) return nullptr;

	return (T*)iter->second.Get();
}

template<typename T>
Ptr<T> AssetMgr::Load(const wstring& _Key, const wstring& _RelativePath) {
	// 동일키로 먼저 등록된 에셋이 있는지 확인
	Ptr<T> pAsset = Find<T>(_Key);

	// 동일키로 먼저 등록된 에셋이 있으면, 그걸 반환
	if (pAsset != nullptr) return pAsset;

	// 에셋 객체 생성
	pAsset = new T;

	// 입력된 경로로부터 에셋 로딩작업 진행	
	pAsset->Load(CONTENT_PATH + _RelativePath);

	// T 타입에 해당하는 실제 AssetType 확인
	EAsset::Type type = GetAssetType<T>();

	// 맵에 에셋등록
	m_mapAsset[type].insert(make_pair(_Key, pAsset.Get()));

	// 에셋이 자신이 매니저에 등롣될때 상요된 Key 와, 
	// 자신이 어떤 경로에 있는 파일로부터 로딩된 에셋인지 스스로 알 수 있도록 해줌
	pAsset->SetKey(_Key);
	pAsset->SetRelativePath(_RelativePath);

	m_Changed = true;

	return pAsset;
}

template<typename T>
Ptr<T> LoadAssetRef(FILE* _File) {
	// Asset 이 Null 인지 아닌지 저장
	bool IsNull = false;
	fread(&IsNull, sizeof(bool), 1, _File);

	// Asset 의 Key, RelativePath 저장
	if (IsNull) {
		wstring Key = LoadWString(_File);
		wstring RelativePath = LoadWString(_File);
		return AssetMgr::GetInst()->Load<T>(Key, RelativePath);
	}

	return nullptr;
}

#define FIND(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)
#define LOAD(Type, AssetPath) AssetMgr::GetInst()->Load<Type>(AssetPath, AssetPath)

#endif // __ASSETMGR_INL__