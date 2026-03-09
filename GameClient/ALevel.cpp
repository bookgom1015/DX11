#include "pch.h"
#include "ALevel.h"

ALevel::ALevel() : Asset(EAsset::E_Level), m_Matrix{} , m_Changed(false) {
	for (int i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].m_LayerIdx = i;
}

ALevel::~ALevel() {}

void ALevel::AddObject(int _LayerIdx, Ptr<GameObject> _Object) {
	m_arrLayer[_LayerIdx].AddObject(_Object);
}

void ALevel::Deregister() {
	for (UINT i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].DeregisterObject();
}

void ALevel::Begin() {
	for (UINT i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].Begin();
}

void ALevel::Tick() {
	for (UINT i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].Tick();
}

void ALevel::LateTick() {
	for (UINT i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].LateTick();
}

void ALevel::FinalTick() {
	for (UINT i = 0; i < MAX_LAYER; ++i)
		m_arrLayer[i].FinalTick();
}

int ALevel::Save(const wstring& _FilePath) {
	FILE* file{};
	_wfopen_s(&file, _FilePath.c_str(), L"wb");

	// 레벨 이름
	auto levelName = GetName();
	SaveWString(file, levelName);

	// 충돌 체크 정보
	fwrite(m_Matrix, sizeof(UINT), MAX_LAYER, file);

	// 레이어 정보
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		// 레이어 이름 저장
		auto layerName = m_arrLayer[i].GetName();
		SaveWString(file, layerName);

		// 레이어 내 최상위 개체를 계층 구조로 저장
		decltype(auto) parents = m_arrLayer[i].GetParentObjects();

		auto numParents = parents.size();
		fwrite(&numParents, sizeof(numParents), 1, file);

		for (const auto& object : parents) 
			object->SaveToLevelFile(file);
	}

	fclose(file);

	return S_OK;
}

int ALevel::Load(const wstring& _FilePath) {
	FILE* file{};
	_wfopen_s(&file, _FilePath.c_str(), L"rb");

	// 레벨 이름
	auto levelName = LoadWString(file);
	SetName(levelName);

	// 충돌 체크 정보
	fread(m_Matrix, sizeof(UINT), MAX_LAYER, file);

	// 레이어 정보
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		// 레이어 이름 불러오기
		auto layerName = LoadWString(file);
		m_arrLayer[i].SetName(layerName);

		size_t numParents{};
		fread(&numParents, sizeof(numParents), 1, file);

		for (size_t j = 0; j < numParents; ++j) {
			Ptr<GameObject> object = new GameObject;
			object->LoadFromLevelFile(file);
			AddObject(i, object);
		}
	}


	fclose(file);

	return S_OK;
}

void ALevel::CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2) {
	UINT Row = _LayerIdx1;
	UINT Col = _LayerIdx2;

	// 더 작은 레이어 인덱스를 행 으로 사용한다.
	if (_LayerIdx2 < _LayerIdx1) {
		Row = _LayerIdx2;
		Col = _LayerIdx1;
	}
	
	m_Matrix[Row] ^= (1 << Col);
}

void ALevel::CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2) {}

Ptr<GameObject> ALevel::FindObjectByName(const wstring& _Name) {
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		const vector<Ptr<GameObject>>& vecParents = m_arrLayer[i].GetParentObjects();

		for (size_t i = 0, end = vecParents.size(); i < end; ++i) {
			list<Ptr<GameObject>> queue;
			queue.push_back(vecParents[i]);

			while (!queue.empty()) {
				Ptr<GameObject> pObject = queue.front();
				queue.pop_front();

				// 찾았다
				if (pObject->GetName() == _Name)
					return pObject;

				const vector<Ptr<GameObject>>& vecChild = pObject->GetChild();
				for (size_t j = 0; j < vecChild.size(); ++j)
					queue.push_back(vecChild[j]);
			}
			
		}
	}

	// 없다
	return nullptr;
}