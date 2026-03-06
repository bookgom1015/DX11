#include "pch.h"
#include "AddComponentButton.h"

#include "EditorMgr.h"
#include "Source/ScriptMgr.h"

#include "ListUI.h"

AddComponentButton::AddComponentButton() 
	: ComponentUI(EComponent::E_None, "AddCompBtn") {}

AddComponentButton::~AddComponentButton() {}

void AddComponentButton::Tick_UI() {
	float windowWidth = ImGui::GetContentRegionAvail().x;

	ImGui::SetCursorPosX((windowWidth - mLastButtonsWidth) * 0.5f);
	ImGui::BeginChild("AddCompButtons", Vec2(mLastButtonsWidth, 0.f));

	mLastButtonsWidth = 0.f;

	if (ImGui::Button("Add Component")) AddComponent();
	mLastButtonsWidth += ImGui::GetItemRectSize().x;

	ImGui::SameLine(0.f, 10.f);
	mLastButtonsWidth += 10.f;

	if (ImGui::Button("Add Script")) AddScript();
	mLastButtonsWidth += ImGui::GetItemRectSize().x;

	ImGui::EndChild();
}

void AddComponentButton::SelectScript(DWORD_PTR _ListUI) {
	Ptr<ListUI> pListUI = ((ListUI*)_ListUI);

	wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());

	string msg = format("{} 선택", WStrToStr(key));
	LOG_INFO(msg);
}

void AddComponentButton::AddComponent() {
	
}

void AddComponentButton::AddScript() {
	Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
	assert(pUI.Get());

	pUI->SetUIName("Scripts");

	vector<wstring> scripts;
	ScriptMgr::GetScriptInfo(scripts);

	pUI->AddString(scripts);
	pUI->AddDelegate(this, (DELEGATE_1)&AddComponentButton::SelectScript);
	pUI->SetActive(true);
}