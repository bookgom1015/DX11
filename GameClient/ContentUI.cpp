#include "pch.h"
#include "ContentUI.h"

#include "AssetMgr.h"
#include "EditorMgr.h"

#include "Inspector.h"

#include "TreeUI.h"

ContentUI::ContentUI()
	: EditorUI("Content") {
	m_Tree = NEW TreeUI;
	m_Tree->SetSaperator(false);
	m_Tree->AddDynamicSelect(this, (DELEGATE_1)&ContentUI::SelectAsset);

	AddChildUI(m_Tree.Get());

	// Asset 내용을 트리에 반영
	Renew();
}

ContentUI::~ContentUI() {}

void ContentUI::Tick_UI() {
	if (AssetMgr::GetInst()->IsChanged()) Renew();
}

void ContentUI::Renew() {
	// 트리 비우기
	m_Tree->Clear();

	// 에셋 종류별로 Tree 에 추가하기
	for (UINT i = 0; i < EAsset::Count; ++i) {
		// 에셋의 이름에 해당하는 노드를 추가 (enum 타입을 문자열로 바꿔서 추가)
		Ptr<TreeNode> pNode = m_Tree->AddItem(nullptr, ToString((EAsset::Type)i));
		pNode->SetFramed(true);

		// 해당 에셋 모든 이름을 받아와서 하위 자식으로 추가
		vector<wstring> vecNames;
		AssetMgr::GetInst()->GetAssetNames((EAsset::Type)i, vecNames);

		for (const auto& str : vecNames) {			
			Ptr<Asset> pAsset = AssetMgr::GetInst()->FindAsset((EAsset::Type)i, str);
			m_Tree->AddItem(pNode, string(str.begin(), str.end()), (DWORD_PTR)pAsset.Get());
		}		
	}	
}

void ContentUI::SelectAsset(DWORD_PTR _Asset) {
	if (_Asset == 0) return;

	// 클릭한 노드가 들고있는 Asset 주소값을 입력받는다.
	Ptr<Asset> pAsset = (Asset*)_Asset;

	// Inspector에 ContentUI 에서 클릭된 Asset 의 주소를 알려준다.
	Ptr<Inspector> pInspector = (Inspector*)EditorMgr::GetInst()->FindUI("Inspector").Get();
	assert(pInspector.Get());

	pInspector->SetTargetAsset(pAsset);
}
