#include "pch.h"
#include "TileMapUI.h"

#include "EditorMgr.h"

#include "ATileMap.h"

TileMapUI::TileMapUI()	: AssetUI(EAsset::E_TileMap) {}

TileMapUI::~TileMapUI() {}

void TileMapUI::Tick_UI() {
	OutputTitle();

	Ptr<ATileMap> tileMap = (ATileMap*)GetTargetAsset().Get();

	{
		ImGui::Text("Atlas");
		ImGui::SameLine(100.f);

		ImGui::ImageWithBg(
			tileMap->GetAtlas()->GetSRV().Get(),
			Vec2(200.f), Vec2(0.f), Vec2(1.f), Vec4(0.f, 0.f, 0.f, 1.f));
	}
	{
		ImGui::Text("Reference");
		ImGui::SameLine(100.f);

		auto key = WStrToStr(tileMap->GetAtlas()->GetKey());
		ImGui::SetNextItemWidth(200.f);
		ImGui::InputText("##ATLAS_KEY", &key, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget()) {
			EditorMgr::AcceptAssetDragDrop("Content", EAsset::E_Texture, [&](Ptr<Asset> asset) {
				auto texture = static_cast<ATexture*>(asset.Get());
				tileMap->SetAtlas(texture);
			});

			ImGui::EndDragDropTarget();
		}
	}
}