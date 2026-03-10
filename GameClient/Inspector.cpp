#include "pch.h"
#include "Inspector.h"

#include "LevelMgr.h"
#include "TaskMgr.h"
#include "EditorMgr.h"

#include "GameObject.h"

Inspector::Inspector() : EditorUI("Inspector") {
	CreateChildUI();
	SetTargetObject(nullptr);	
}

Inspector::~Inspector() {}

void Inspector::SetTargetObject(Ptr<GameObject> _Object) {
	// 입력된 게임오브젝트의 정보를 보여줄 ComponentUI 들을 활성화 시킨다.
	m_TargetObject = _Object;

	m_AddCompBtn->SetTarget(m_TargetObject);

	for (UINT i = 0; i < EComponent::Count; ++i) {
		if (m_arrComUI[i] == nullptr) continue;

		m_arrComUI[i]->SetTarget(m_TargetObject);
	}

	for (UINT i = 0; i < SCRIPT_TYPE::Count; ++i) {
		if (m_arrScriptUI[i] == nullptr) continue;

		m_arrScriptUI[i]->SetTarget(m_TargetObject);
	}
		
	// AssetUI 를 비활성화한다.
	m_TargetAsset = nullptr;
	for (UINT i = 0; i < EAsset::Count; ++i) {
		if(m_arrAssetUI[i] != nullptr) m_arrAssetUI[i]->SetActive(false);
	}
}

void Inspector::SetTargetAsset(Ptr<Asset> _Asset) {
	// ComponentUI 들을 비활성화 시킨다.
	SetTargetObject(nullptr);

	// 입력된 에셋 담당 UI 를 활성화시킨다.
	m_TargetAsset = _Asset;
	if (m_TargetAsset == nullptr) {
		for (UINT i = 0; i < EAsset::Count; ++i)
			m_arrAssetUI[i]->SetActive(false);
	}
	else {
		EAsset::Type Type = m_TargetAsset->GetType();
		m_arrAssetUI[Type]->SetActive(true);
		m_arrAssetUI[Type]->SetTargetAsset(m_TargetAsset);
	}
}

void Inspector::NeedToResetTarget() {
	if (m_TargetObject == nullptr) return;

	auto name = m_TargetObject->GetName();

	//auto func = MAKE_DEFERRED_TASK({
	//	auto target = LevelMgr::GetInst()->FindObjectByName(name);
	//	SetTargetObject(target);
	//}, &, name);

	TaskInfo info{};
	info.Type = ETask::E_DeferredProcessing;
	info.Param_0 = DWORD_PTR_DEFERRED_TASK({
		auto target = LevelMgr::GetInst()->FindObjectByName(name);
		SetTargetObject(target);
	}, &, name);

	TaskMgr::GetInst()->AddTask(info);
}

void Inspector::Tick_UI() {
	if (m_TargetObject == nullptr) return; 

	wstring Name = m_TargetObject->GetName();
	string strName = string(Name.begin(), Name.end());
	if (strName.empty()) strName = "No Name";

	ImGui::Button(strName.c_str());

	ImGui::Separator();
}