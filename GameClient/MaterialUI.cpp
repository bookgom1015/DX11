#include "pch.h"
#include "MaterialUI.h"

#include "PathMgr.h"
#include "AMaterial.h"
#include "EditorMgr.h"
#include "ListUI.h"
#include "AssetMgr.h"

MaterialUI::MaterialUI()
	: AssetUI(ASSET_TYPE::MATERIAL)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::Tick_UI()
{
	OutputTitle();

	Ptr<AMaterial> pMtrl = (AMaterial*)GetTargetAsset().Get();

	string Key = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());

	ImGui::Text("Name");
	ImGui::SameLine(100);
	ImGui::InputText("##MtrlName", Key.data(), Key.length() + 1, ImGuiInputTextFlags_ReadOnly);

	// ======
	// Shader
	// ======
	ImGui::Text("Shader");
	ImGui::SameLine(100);

	Ptr<AGraphicShader> pShader = pMtrl->GetShader();

	wstring ShaderKey = L"None";
	if (nullptr != pShader)
	{
		ShaderKey = pShader->GetKey();
	}
	
	ImGui::InputText("##ShaderName", string(ShaderKey.begin(), ShaderKey.end()).data(), ShaderKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

	// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content");
		if (PayLoad)
		{
			DWORD_PTR data = *((DWORD_PTR*)PayLoad->Data);
			Ptr<Asset> pAsset = (Asset*)data;

			if (ASSET_TYPE::GRAPHICSHADER == pAsset->GetType())
			{
				pMtrl->SetShader((AGraphicShader*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##ShaderBtn", Vec2(20.f, 20.f)))
	{
		// 버튼이 눌리면, 리스트UI 를 찾아서 활성화 시키고, 출력시키고 싶은 문자열을 ListUI 에 등록시킨다.
		Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
		assert(pUI.Get());

		pUI->SetUIName("Shader List");

		vector<wstring> vecShaderNames;
		AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::GRAPHICSHADER, vecShaderNames);
		pUI->AddString(vecShaderNames);
		pUI->AddDelegate(this, (DELEGATE_1)&MaterialUI::SelectShader);
		pUI->SetActive(true);
	}


	// Save Button
	if (ImGui::Button("Save##MtrlSaveBtn"))
	{
		wstring FilePath = CONTENT_PATH + pMtrl->GetKey();
		pMtrl->Save(FilePath);
	}
}

void MaterialUI::SelectShader(DWORD_PTR _ListUI)
{
	Ptr<ListUI> pListUI = ((ListUI*)_ListUI);

	wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());
		
	Ptr<AGraphicShader> pShader = FIND(AGraphicShader, key);

	((AMaterial*)GetTargetAsset().Get())->SetShader(pShader);
}
