#include "pch.h"
#include "AssetUI.h"

#include "AssetMgr.h"
#include "PathMgr.h"
#include "EditorMgr.h"

AssetUI::AssetUI(EAsset::Type _Type) : EditorUI(ToString(_Type)), m_AssetType(_Type) {
	SetActive(false);
	SetSaperator(false);
}

AssetUI::~AssetUI() {}

void AssetUI::OutputTitle() {
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::Button(ToString(m_AssetType));
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SameLine();

	// 오른쪽으로 커서 이동
	EditorMgr::RightAlignNextItem({ "Revert", "Save" });

	auto asset = GetTargetAsset();
	if (ImGui::Button("Revert"))  {
		auto key = asset->GetKey();
		switch (asset->GetType()) {
		case EAsset::E_Mesh: {
			auto loadedAsset = FORCE_LOAD(AMesh, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Material: {
			auto loadedAsset = FORCE_LOAD(AMaterial, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Texture: {
			auto loadedAsset = FORCE_LOAD(ATexture, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Sound: 
			break;
		case EAsset::E_GraphicShader:
			break;
		case EAsset::E_ComputeShader:
			break;
		case EAsset::E_Level: {
			auto loadedAsset = FORCE_LOAD(ALevel, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Sprite: {
			auto loadedAsset = FORCE_LOAD(ASprite, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Flipbook: {
			auto loadedAsset = FORCE_LOAD(AFlipbook, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_TileMap: {
			auto loadedAsset = FORCE_LOAD(ATileMap, key);
			SetTargetAsset(loadedAsset.Get());
		}
			break;
		case EAsset::E_Prefab:
			break;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) 
		asset->Save(format(L"{}{}", CONTENT_PATH, asset->GetKey()));


	ImGui::Spacing();
	ImGui::Spacing();
}
