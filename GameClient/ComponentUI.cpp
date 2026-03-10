#include "pch.h"
#include "ComponentUI.h"

#include "TaskMgr.h"

#include "ScriptUI.h"

ComponentUI::ComponentUI(EComponent::Type _Type, const string& _Name)
	: EditorUI(_Name), m_ComType(_Type) {}

ComponentUI::~ComponentUI() {}

void ComponentUI::SetTarget(Ptr<GameObject> _Object) {
	m_Target = _Object;

	if (m_ComType == EComponent::E_None) {
		SetActive(m_Target != nullptr);
		return;
	}
	else if (m_ComType != EComponent::E_Script) {
		if (m_Target == nullptr || m_Target->GetComponent(m_ComType) == nullptr)
			SetActive(false);
		else
			SetActive(true);
	}
}

void ComponentUI::OutputTitle(const string& _Title) {
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::Button(_Title.c_str());
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	if (ImGui::Button(format("X##{}", _Title).c_str())) RemoveComponent();
	ImGui::PopID();

	ImGui::Spacing();
	ImGui::Spacing();
}

void ComponentUI::RemoveComponent() {
	TaskInfo task{};
	task.Type = ETask::E_DeferredProcessing;

	if (m_ComType == EComponent::E_Script) {
		auto scriptUI = static_cast<ScriptUI*>(this);

		task.Param_0 = DWORD_PTR_DEFERRED_TASK({
			m_Target->RemoveComponent(m_ComType, scriptUI->GetScriptType());
			SetActive(false);
		}, &, scriptUI);
	}
	else {
		task.Param_0 = DWORD_PTR_DEFERRED_TASK({
			m_Target->RemoveComponent(m_ComType);
			SetActive(false);
		}, &);
	}

	TaskMgr::GetInst()->AddTask(task);
}