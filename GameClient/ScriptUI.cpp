#include "pch.h"
#include "ScriptUI.h"

#include "EditorMgr.h"

ScriptUI::ScriptUI(SCRIPT_TYPE type) 
	: ComponentUI(EComponent::E_Script, "ScriptUI"), m_ScriptType(type) {}

ScriptUI::~ScriptUI() {}

void ScriptUI::SetTarget(Ptr<GameObject> _Object) {
	ComponentUI::SetTarget(_Object);

	if (GetTarget() == nullptr || GetTarget()->GetScript(m_ScriptType) == nullptr)
		SetActive(false);
	else
		SetActive(true);
}

void ScriptUI::Tick_UI() {
	OutputTitle(WStrToStr(ScriptMgr::GetScriptName(m_ScriptType)));

	auto script = GetTarget()->GetScript(m_ScriptType);

	vector<Property> properties{};
	script->GetProperties(properties);

	for (const auto& property : properties) {
		switch (property.Type) {
		case Property::Bool: PropertyBool(script.Get(), property); break;
		case Property::Int: PropertyInt(script.Get(), property); break;
		case Property::Float: PropertyFloat(script.Get(), property); break;
		case Property::Vec2: PropertyVec2(script.Get(), property);break;
		case Property::Vec3: PropertyVec3(script.Get(), property);break;
		case Property::Object: PropertyObject(script.Get(), property); break;
		}
	}
}

void ScriptUI::PropertyBool(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;

	if (ImGui::BeginTable("PropertyBool", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##PropCheckBox_Bool", static_cast<bool*>(data));

		ImGui::EndTable();
	}
}

void ScriptUI::PropertyInt(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;

	if (ImGui::BeginTable("PropertyInt", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragInt(
			"##PropCheckBox_Int"
			, static_cast<int*>(data)
			, 1.f);

		ImGui::EndTable();
	}
}

void ScriptUI::PropertyFloat(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;

	if (ImGui::BeginTable("PropertyFloat", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat(
			"##PropCheckBox_Float"
			, static_cast<float*>(data)
			, 1.f);

		ImGui::EndTable();
	}
}

void ScriptUI::PropertyVec2(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;

	if (ImGui::BeginTable("PropertyVec2", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat2(
			"##PropCheckBox_Vec2"
			, static_cast<float*>(data)
			, 1.f);

		ImGui::EndTable();
	}
}

void ScriptUI::PropertyVec3(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;

	if (ImGui::BeginTable("PropertyVec3", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat3(
			"##PropCheckBox_Vec3"
			, static_cast<float*>(data)
			, 1.f);

		ImGui::EndTable();
	}
}

void ScriptUI::PropertyObject(CScript* const script, const Property& prop) {
	void* data = reinterpret_cast<BYTE*>(script) + prop.Offset;
	Ptr<GameObject>* pObject = reinterpret_cast<Ptr<GameObject>*>(data);
	Ptr<GameObject> object = *pObject;

	if (ImGui::BeginTable("PropertyObject", 2)) {
		ImGui::TableNextColumn();
		ImGui::Text(prop.Name.c_str());
		
		std::string name{};
		if (object != nullptr) {
			decltype(auto) nameW = object->GetName();
			name = WStrToStr(nameW);
		}

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputText(
			"##PropCheckBox_Object",
			name.data(), name.length() + 1
			, ImGuiInputTextFlags_ReadOnly);

		ImGui::EndTable();
	}
}