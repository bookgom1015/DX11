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
		ImGui::Text("TileMap");
		ImGui::SameLine(100.f);

		const auto rectSize = Vec2(200.f);

		ImGui::BeginChild("TileMap_Sprites", rectSize);

		auto rows = tileMap->GetRow();
		auto cols = tileMap->GetCol();

		auto leftTopPos = EditorMgr::GetCursorScreenPos();

		Vec2 spriteSize = Vec2(
			rectSize.x / static_cast<float>(cols),
			rectSize.y / static_cast<float>(rows));

		for (UINT row = 0; row < rows; ++row) {
			for (UINT col = 0; col < cols; ++col) {
				auto newPos = leftTopPos + Vec2(col * spriteSize.x, row * spriteSize.y);

				ImGui::SetCursorScreenPos(newPos);

				auto sprite = tileMap->GetSprite(row, col);

				auto leftTopUV = sprite->GetLeftTopUV();
				auto sliceUV = sprite->GetSliceUV();
				auto backgroundUV = sprite->GetBackgroundUV();
				auto offsetUV = sprite->GetOffsetUV();

				auto actualLeftTopUV = leftTopUV + sliceUV * 0.5f - backgroundUV * 0.5f;
				actualLeftTopUV += offsetUV;

				ImGui::ImageWithBg(
					tileMap->GetAtlas()->GetSRV().Get()
					, spriteSize
					, actualLeftTopUV
					, actualLeftTopUV + backgroundUV
					, Vec4(0.f));
			}
		}


		ImGui::EndChild();
	}
	{
		ImGui::Text("Name");
		ImGui::SameLine(100.f);

		string key = WStrToStr(tileMap->GetKey());

		ImGui::SetNextItemWidth(200.f);
		ImGui::InputText("##TILEMAP_KEY", &key, ImGuiInputTextFlags_ReadOnly);
	}

	ImGui::Dummy(Vec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(Vec2(0.f, 20.f));

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