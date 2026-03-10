#include "pch.h"
#include "FlipbookEditorUI.h"

#include "AssetMgr.h"
#include "EditorMgr.h"

namespace {
	const float SpritesViewHeight = 160.f;
	const float SpriteImageSize = 120.f;
}

FlipbookEditorUI::FlipbookEditorUI()
	: EditorUI("Flipbook Editor")
	, m_Grid{ 1, 1 }
	, m_BackgroundUV{} {}

FlipbookEditorUI::~FlipbookEditorUI() {}

void FlipbookEditorUI::Tick_UI() {
	auto avail = ImGui::GetContentRegionAvail();

	// Upper
	{
		ImGui::BeginChild("Upper", ImVec2(0.f, avail.y - SpritesViewHeight));

		if (ImGui::BeginTable("FlipbookEditorTable", 2)) {
			ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthStretch, 8.f);
			ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthStretch, 2.f);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			LeftPanel();

			ImGui::TableSetColumnIndex(1);
			RightPanel();

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}
	// Lower
	{
		auto size = ImGui::GetStyle().ScrollbarSize;
		ImGui::BeginChild("Lower", ImVec2(0.f, SpritesViewHeight - size));

		LowerPanel();

		ImGui::EndChild();
	}
}

void FlipbookEditorUI::LeftPanel() {
	auto avail = ImGui::GetContentRegionAvail();
	ImGui::ImageWithBg(
		m_Texture != nullptr ? m_Texture->GetSRV().Get() : nullptr
		, avail
		, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 1.f));

	if (m_Texture != nullptr) {
		ImVec2 imageMin = ImGui::GetItemRectMin();
		ImVec2 imageMax = ImGui::GetItemRectMax();

		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->PushClipRect(imageMin, imageMax, true);

		const ImU32 innerColor = IM_COL32(0, 255, 0, 255);
		const ImU32 outerColor = IM_COL32(0, 0, 0, 200);
		const float innerThick = 1.f;
		const float outerThick = innerThick * 4.f + 1.f;

		int gridCols = m_Grid[1];
		int gridRows = m_Grid[0];

		auto imageSize = ImVec2(imageMax.x - imageMin.x, imageMax.y - imageMin.y);
		const float cellW = imageSize.x / static_cast<float>(gridCols);
		const float cellH = imageSize.y / static_cast<float>(gridRows);

		// 세로선
		for (int x = 0; x <= gridCols; ++x) {
			float px = imageMin.x + x * cellW;
			dl->AddLine(
				ImVec2(px, imageMin.y),
				ImVec2(px, imageMax.y),
				outerColor,
				outerThick
			);
		}

		// 가로선
		for (int y = 0; y <= gridRows; ++y) {
			float py = imageMin.y + y * cellH;
			dl->AddLine(
				ImVec2(imageMin.x, py),
				ImVec2(imageMax.x, py),
				outerColor,
				outerThick
			);
		}

		// 세로선
		for (int x = 0; x <= gridCols; ++x) {
			float px = imageMin.x + x * cellW;
			dl->AddLine(
				ImVec2(px, imageMin.y),
				ImVec2(px, imageMax.y),
				innerColor,
				innerThick
			);
		}

		// 가로선
		for (int y = 0; y <= gridRows; ++y) {
			float py = imageMin.y + y * cellH;
			dl->AddLine(
				ImVec2(imageMin.x, py),
				ImVec2(imageMax.x, py),
				innerColor,
				innerThick
			);
		}

		dl->PopClipRect();
	}

	if (ImGui::BeginDragDropTarget()) {
		decltype(auto) PayLoad = ImGui::AcceptDragDropPayload("Content");
		if (PayLoad) {
			auto data = *(static_cast<DWORD_PTR*>(PayLoad->Data));
			Ptr<Asset> asset = reinterpret_cast<Asset*>(data);

			if (asset->GetType() == EAsset::E_Texture)
				m_Texture = static_cast<ATexture*>(asset.Get());
		}

		ImGui::EndDragDropTarget();
	}
}

void FlipbookEditorUI::RightPanel() {
	// 텍스처 크기
	{
		ImGui::Text("Texture Size");
		ImGui::SameLine(100.f);

		int size[2] = {};
		if (m_Texture != nullptr) {
			size[0] = static_cast<int>(m_Texture->GetWidth());
			size[1] = static_cast<int>(m_Texture->GetHeight());
		}
		ImGui::InputInt2("##TEXTURE_SIZE", size, ImGuiInputTextFlags_ReadOnly);
	}
	// 하나의 스프라이트 크기
	{
		ImGui::Text("Sprite Size");
		ImGui::SameLine(100.f);

		int size[2] = {};
		if (m_Texture != nullptr) {
			size[0] = static_cast<int>(m_Texture->GetWidth()) / m_Grid[1];
			size[1] = static_cast<int>(m_Texture->GetHeight()) / m_Grid[0];
		}
		ImGui::InputInt2("##SPRITE_SIZE", size, ImGuiInputTextFlags_ReadOnly);
	}
	// Slice UV
	{
		auto texWidth = 0.f;
		auto texHeight = 0.f;
		if (m_Texture != nullptr) {
			texWidth = m_Texture->GetWidth();
			texHeight = m_Texture->GetHeight();
		}

		auto spriteSizeX = texWidth / static_cast<float>(m_Grid[1]);
		auto spriteSizeY = texHeight / static_cast<float>(m_Grid[0]);

		auto slice = Vec2(spriteSizeX / texWidth, spriteSizeY / texHeight);

		ImGui::Text("Slice UV");
		ImGui::SameLine(100.f);

		ImGui::InputFloat2("##SLICE_UV", slice, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 그리드 크기
	{
		ImGui::Text("Grid Size");
		ImGui::SameLine(100.f);

		ImGui::DragInt2("##GRID_SIZE", m_Grid, 0.05f, 1, 64);
	}
	// Background UV Size
	{
		ImGui::Text("Background UV");
		ImGui::SameLine(100.f);

		ImGui::DragFloat2("##BACKGROUND_UV", m_BackgroundUV);
	}

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 버튼들
	{
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		float applyWidth = ImGui::CalcTextSize("Apply").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float saveWidth = ImGui::CalcTextSize("Save").x + ImGui::GetStyle().FramePadding.x * 2.0f;

		float totalWidth = applyWidth + spacing + saveWidth;

		// 오른쪽으로 커서 이동
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - totalWidth + ImGui::GetCursorPosX());

		if (ImGui::Button("Apply")) MakeSprites();
		ImGui::SameLine();
		if (ImGui::Button("Save")) SaveSprites();
	}
}

void FlipbookEditorUI::LowerPanel() {
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const int count = static_cast<int>(m_Sprites.size());

	// 가로 전체 콘텐츠 폭 계산
	const float totalWidth = count * SpriteImageSize + (count - 1) * spacing;

	const auto scrollBarSize = ImGui::GetStyle().ScrollbarSize;

	// 스크롤 영역 생성
	ImGui::BeginChild(
		"LowerPanelScroll",
		ImVec2(0.f, SpritesViewHeight - scrollBarSize), // 높이만 적당히 지정, 너비 0이면 부모 너비 사용
		ImGuiChildFlags_None,
		ImGuiWindowFlags_HorizontalScrollbar);

	// 내부 콘텐츠 폭을 강제로 넓혀서 가로 스크롤이 생기게 함
	ImGui::SetCursorPosX(0.f);
	ImGui::InvisibleButton("##CONTNET_WIDTH", ImVec2(totalWidth, 1.f));
	ImGui::SetCursorPos(ImVec2(0.f, 0.f));

	for (size_t i = 0; i < count; ++i) {
		if (i != 0) ImGui::SameLine();

		auto sprite = m_Sprites[i];

		Vec2 leftTopUV = sprite->GetLeftTopUV();
		Vec2 sliceUV = sprite->GetSliceUV();

		ImGui::ImageWithBg(
			(ImTextureRef)sprite->GetAtlas()->GetSRV().Get()
			, ImVec2(SpriteImageSize, SpriteImageSize)
			, leftTopUV, leftTopUV + sliceUV
			, ImVec4(0.f, 0.f, 0.f, 1.f));
	}

	ImGui::EndChild();
}

void FlipbookEditorUI::MakeSprites() {
	m_Sprites.clear();

	auto texWidth = m_Texture->GetWidth();
	auto texHeight = m_Texture->GetHeight();

	auto spriteSizeX = texWidth / static_cast<float>(m_Grid[1]);
	auto spriteSizeY = texHeight / static_cast<float>(m_Grid[0]);

	for (int i = 0, iend = m_Grid[1]; i < iend; ++i) {
		for (int j = 0, jend = m_Grid[0]; j < jend; ++j) {
			Ptr<ASprite> sprite = NEW ASprite();
			sprite->SetAtlas(m_Texture);

			auto slice = Vec2(spriteSizeX / texWidth, spriteSizeY / texHeight);
			auto leftTop = Vec2(slice.x * i, 0.f) + Vec2(0.f, slice.y * j);

			sprite->SetSliceUV(slice);
			sprite->SetLeftTopUV(leftTop);

			m_Sprites.push_back(sprite);
		}
	}
}

void FlipbookEditorUI::SaveSprites() {

}