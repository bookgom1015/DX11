#include "pch.h"
#include "TileRenderUI.h"

#include "EditorMgr.h"

TileRenderUI::TileRenderUI() 
	: ComponentUI(EComponent::E_TileRender, "TileRenderUI") {}

TileRenderUI::~TileRenderUI() {}


void TileRenderUI::Tick_UI() {
	OutputTitle("TileRender");

	auto comp = GetTarget()->GetComponent(EComponent::E_TileRender);
	auto tileRender = static_cast<CTileRender*>(comp.Get());

	auto tileMap = tileRender->GetTileMap();

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

		if (ImGui::BeginDragDropTarget()) {
			EditorMgr::AcceptAssetDragDrop("Content", EAsset::E_TileMap, [&](Ptr<Asset> asset) {
				auto newTileMap = static_cast<ATileMap*>(asset.Get());
				tileRender->SetTileMap(newTileMap);
			});

			ImGui::EndDragDropTarget();
		}
	}
}