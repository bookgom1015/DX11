#include "pch.h"
#include "EditorUI.h"

#include "EditorMgr.h"

#include "ImGui/imgui.h"


EditorUI::EditorUI(const string& _Name)
	: m_UIName(_Name)
	, m_IsModal(false)
	, m_Separator(true)
	, m_Active(true)
	, m_Parent(nullptr) {}

EditorUI::~EditorUI() {}

void EditorUI::Tick() {
	if (m_IsModal) {
		string StrKey = m_UIName + m_UIKey;
		ImGui::OpenPopup(StrKey.c_str());

		bool Active = m_Active;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30.f, 20.f));
		if (ImGui::BeginPopupModal(
			StrKey.c_str(), &Active, ImGuiWindowFlags_AlwaysAutoResize)) {
			CheckFocus();

			Tick_UI();

			for (size_t i = 0, end = m_ChildUI.size(); i < end; ++i) {
				if (m_ChildUI[i]->IsActive()) {
					m_ChildUI[i]->Tick();
					ImGui::Separator();
				}
			}

			ImGui::EndPopup();
		}
		else {			
			SetActive(Active);
		}
		ImGui::PopStyleVar();
	}
	else if (m_Parent == nullptr) {
		bool Active = m_Active;

		string StrKey = m_UIName + m_UIKey;

		ImGui::Begin(StrKey.c_str(), &Active);

		if (m_Active != Active) SetActive(Active);

		CheckFocus();

		Tick_UI();

		for (size_t i = 0, end = m_ChildUI.size(); i < end; ++i) 
			if (m_ChildUI[i]->IsActive()) m_ChildUI[i]->Tick();

		ImGui::End();		
	}
	else {
		//ImGui::BeginChild(m_UIName.c_str(), m_SizeAsChild);

		CheckFocus();

		Tick_UI();

		for (size_t i = 0, end = m_ChildUI.size(); i < end; ++i) {
			if (m_ChildUI[i]->IsActive()) {
				m_ChildUI[i]->Tick();
				ImGui::Separator();
			}				
		}

		//ImGui::EndChild();

		ImGui::Dummy(m_SizeAsChild);

		if (m_Separator) ImGui::Separator();
	}	
}

void EditorUI::CheckFocus() {
	if (ImGui::IsWindowFocused())
		EditorMgr::GetInst()->RegisterFocusedUI(this);

	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		EditorMgr::GetInst()->RegisterFocusedUI(this);
}

Vec2::operator ImVec2() const {
	return ImVec2(x, y);
}

Vec4::operator ImVec4() const {
	return ImVec4(x, y, z, w);
}
