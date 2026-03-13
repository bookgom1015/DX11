#include "pch.h"
#include "AtlasEditorUI.h"

#include "AssetMgr.h"
#include "KeyMgr.h"
#include "EditorMgr.h"

namespace {
	const float SpritesViewHeight = 160.f;
	const float SpriteImageSize = 120.f;
}

AtlasEditorUI::AtlasEditorUI()
	: EditorUI("Atlas Editor")
	, m_Grid{ 1, 1 }
	, m_BackgroundUV{} {}

AtlasEditorUI::~AtlasEditorUI() {}

void AtlasEditorUI::Tick_UI() {
	auto avail = ImGui::GetContentRegionAvail();
	auto size = ImGui::GetStyle().ScrollbarSize;

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
		ImGui::BeginChild("Lower", ImVec2(0.f, SpritesViewHeight - size));

		LowerPanel();

		ImGui::EndChild();
	}
}

void AtlasEditorUI::LeftPanel() {
	auto leftTopPos = ImGui::GetCursorScreenPos();

	// 아틀라스 이미지
	AtlasImage();

	auto imageRectSize = ImGui::GetItemRectSize();
	auto imageMin = ImGui::GetItemRectMin();
	auto imageMax = ImGui::GetItemRectMax();

	// 그리드 선
	Grid(imageMin, imageMax);

	// 아틀라스 위 그리드 선택 영역
	SelectableOnGrid(leftTopPos, imageRectSize);

	BackgroundUV(imageMin, imageMax);
}

void AtlasEditorUI::AtlasImage() {
	auto avail = ImGui::GetContentRegionAvail();
	// 텍스처 할당 시 수직 스크롤 생성 방지
	avail.y = max(1.f, avail.y - ImGui::GetStyle().CellPadding.y * 2.f);

	ImGui::ImageWithBg(
		m_Texture != nullptr ? m_Texture->GetSRV().Get() : nullptr
		, avail
		, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 1.f));

	if (ImGui::BeginDragDropTarget()) {
		EditorMgr::AcceptAssetDragDrop("Content", EAsset::E_Texture, [&](Ptr<Asset> asset) {
			m_Texture = static_cast<ATexture*>(asset.Get());
			});

		ImGui::EndDragDropTarget();
	}
}

void AtlasEditorUI::Grid(ImVec2 imageMin, ImVec2 imageMax) {
	if (m_Texture == nullptr) return;

	const ImU32 innerColor = IM_COL32(0, 255, 0, 255);
	const ImU32 outerColor = IM_COL32(0, 0, 0, 200);
	const float innerThick = 1.f;
	const float outerThick = innerThick * 4.f + 1.f;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(imageMin, ImVec2(imageMax.x + outerThick, imageMax.y + outerThick), true);

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

void AtlasEditorUI::BackgroundUV(ImVec2 imageMin, ImVec2 imageMax) {
	if (m_Sprites.empty() || m_SelectedTileIndices.empty()) return;

	auto texWidth = imageMax.x - imageMin.x;
	auto texHeight = imageMax.y - imageMin.y;

	auto offset = m_Sprites[m_SelectedTileIndices.front()].second->GetOffsetUV();
	auto delta = offset * Vec2(texWidth, texHeight);

	auto spriteWidth = texWidth / static_cast<float>(m_Grid[1]);
	auto spriteHeight = texHeight / static_cast<float>(m_Grid[0]);

	auto backgroundWidth = texWidth * m_BackgroundUV.x;
	auto backgroundHeight = texHeight * m_BackgroundUV.y;

	auto diff = Vec2(backgroundWidth - spriteWidth, backgroundHeight - spriteHeight);
		
	auto index = m_SelectedTileIndices.front();

	int row = index / m_Grid[1];
	int col = index % m_Grid[1];
	auto newPos = Vec2(imageMin.x + col * spriteWidth, imageMin.y + row * spriteHeight);
	newPos -= diff * 0.5f;
	newPos += delta;

	auto size = newPos + Vec2(spriteWidth, spriteHeight);
	size += diff;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(imageMin, imageMax, true);

	dl->AddRect(
		newPos
		, size
		, IM_COL32(255, 255, 0, 200)
		, 0.f, 0, 3.f);

	dl->PopClipRect();
}

void AtlasEditorUI::SelectableOnGrid(ImVec2 leftTopPos, ImVec2 imageRectSize) {
	if (m_Texture == nullptr) return;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginGroup();

	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	int rows = m_Grid[0];
	int cols = m_Grid[1];
	int numTiles = rows * cols;

	m_TileRects.clear();
	m_TileRects.reserve(numTiles);
	m_HoveredTiles.clear();

	const Vec2 spriteSize = Vec2(
		imageRectSize.x / static_cast<float>(cols),
		imageRectSize.y / static_cast<float>(rows));

	// 전체 그리드 영역
	SimpleRect gridRect(
		Vec2(leftTopPos.x, leftTopPos.y),
		Vec2(leftTopPos.x + imageRectSize.x, leftTopPos.y + imageRectSize.y));

	const Vec2 mousePos = Vec2(io.MousePos.x, io.MousePos.y);

	// -------------------------
	// 드래그 시작 판정
	// -------------------------
	const bool mouseInGrid = Util::MouseInRect(mousePos, gridRect.Min, gridRect.Max);

	if (!m_DragSelect.Dragging
		&& mouseInGrid
		&& ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (!ImGui::GetIO().KeyShift) m_SelectedTileIndices.clear();

		m_DragSelect = {
			.Dragging = true,
			.BoxSelecting = true,
			.Start = mousePos,
			.End = mousePos };
	}

	// -------------------------
	// 타일 생성
	// -------------------------
	for (int i = 0; i < numTiles; ++i) {
		int row = i / cols;
		int col = i % cols;

		Vec2 newPos(
			leftTopPos.x + col * spriteSize.x,
			leftTopPos.y + row * spriteSize.y);

		bool excepted = !m_Sprites.empty() && !m_Sprites[i].first;
		if (excepted) {
			ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 44, 48, 128));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 44, 48, 200));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 44, 48, 255));

			ImDrawList* dl = ImGui::GetWindowDrawList();
			dl->AddRectFilled(newPos
				, newPos + spriteSize
				, IM_COL32(255, 44, 48, 128));
		}

		ImGui::SetCursorScreenPos(newPos);

		bool selected = IsTileSelected(i);
		if (ImGui::Selectable(
			format("##TILE_{}", i).c_str()
			, &selected
			, ImGuiSelectableFlags_None
			, spriteSize)) {
			// 클릭했는데 드래그 아니면 단일 타일 적용
			if (!m_DragSelect.BoxSelecting) SelectTile(i);
		}

		if (excepted) ImGui::PopStyleColor(3);

		m_TileRects.emplace_back(newPos, newPos + spriteSize);
	}

	// -------------------------
	// 드래그 중 처리
	// -------------------------
	if (m_DragSelect.Dragging) {
		m_DragSelect.End = mousePos;

		const float dx = m_DragSelect.End.x - m_DragSelect.Start.x;
		const float dy = m_DragSelect.End.y - m_DragSelect.Start.y;
		const float dragDistSq = dx * dx + dy * dy;

		// 살짝 움직인 건 클릭 취급, 어느 정도 이상 움직여야 박스 선택
		if (dragDistSq > 9.f) m_DragSelect.BoxSelecting = true;

		if (m_DragSelect.BoxSelecting) {
			Vec2 boxMin(
				(m_DragSelect.Start.x < m_DragSelect.End.x) ? m_DragSelect.Start.x : m_DragSelect.End.x,
				(m_DragSelect.Start.y < m_DragSelect.End.y) ? m_DragSelect.Start.y : m_DragSelect.End.y);

			Vec2 boxMax(
				(m_DragSelect.Start.x > m_DragSelect.End.x) ? m_DragSelect.Start.x : m_DragSelect.End.x,
				(m_DragSelect.Start.y > m_DragSelect.End.y) ? m_DragSelect.Start.y : m_DragSelect.End.y);

			SimpleRect selectRect(boxMin, boxMax);

			// 겹치는 타일 모으기
			for (int i = 0, end = static_cast<int>(m_TileRects.size()); i < end; ++i)
				if (Util::Intersects(selectRect, m_TileRects[i]))
					m_HoveredTiles.push_back(i);

			// 드래그 박스 시각화
			drawList->AddRectFilled(
				selectRect.Min, selectRect.Max,
				IM_COL32(80, 140, 255, 40));

			drawList->AddRect(
				selectRect.Min, selectRect.Max,
				IM_COL32(80, 140, 255, 200));

			// 드래그 중 겹친 타일 강조
			for (int tileIndex : m_HoveredTiles) {
				const SimpleRect& rc = m_TileRects[tileIndex];
				drawList->AddRect(
					rc.Min, rc.Max,
					IM_COL32(0, 255, 0, 255),
					0.0f, 0, 2.0f);
			}
		}

		// -------------------------
		// 드래그 종료
		// -------------------------
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			if (m_DragSelect.BoxSelecting) {
				for (auto tile : m_HoveredTiles) {
					auto begin = m_SelectedTileIndices.begin();
					auto end = m_SelectedTileIndices.end();

					auto iter = std::find(begin, end, tile);

					if (iter == end) {
						m_SelectedTileIndices.emplace_back(tile);
					}
					else {
						iter_swap(iter, end - 1);
						m_SelectedTileIndices.pop_back();
					}
				}
			}

			m_DragSelect.Dragging = false;
			m_DragSelect.BoxSelecting = false;
			m_HoveredTiles.clear();
		}
	}

	ImGui::EndGroup();
	ImGui::PopStyleVar();
}

void AtlasEditorUI::RightPanel() {
	// 자동으로 계산되는 값들 출력
	StatusInfo();

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 그리드 조작
	GridControl();

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 스프라이트
	SpriteControl();

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 버튼들
	ControlButtons();
}

void AtlasEditorUI::StatusInfo() {
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
		auto slice = GetSlice();

		ImGui::Text("Slice UV");
		ImGui::SameLine(100.f);

		ImGui::InputFloat2("##SLICE_UV", slice, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}
}

void AtlasEditorUI::GridControl() {
	// 그리드 크기
	{
		ImGui::Text("Grid Size");
		ImGui::SameLine(100.f);

		if (ImGui::DragInt2("##GRID_SIZE", m_Grid, 0.05f, 1, 64)) {
			m_SelectedTileIndices.clear();
			m_Sprites.clear();

			auto texWidth = 0.f;
			auto texHeight = 0.f;
			if (m_Texture != nullptr) {
				texWidth = m_Texture->GetWidth();
				texHeight = m_Texture->GetHeight();
			}

			auto spriteSizeX = texWidth / static_cast<float>(m_Grid[1]);
			auto spriteSizeY = texHeight / static_cast<float>(m_Grid[0]);

			auto slice = Vec2(0.f, 0.f);
			if (m_Texture != nullptr)
				slice = Vec2(spriteSizeX / texWidth, spriteSizeY / texHeight);

			m_BackgroundUV = slice;
		}
	}
	// Background UV Size
	{
		ImGui::Text("Background UV");
		ImGui::SameLine(100.f);

		auto slice = GetSlice();

		if (ImGui::DragFloat2("##BACKGROUND_UV", m_BackgroundUV, 0.001f)) {
			m_BackgroundUV.x = max(m_BackgroundUV.x, slice.x);
			m_BackgroundUV.y = max(m_BackgroundUV.y, slice.y);
		}
	}
}

void AtlasEditorUI::SpriteControl() {
	ImGui::Text("Sprite Prefix");
	ImGui::SameLine(100.f);

	ImGui::InputText("##SPRITE_PREFIX", &m_Prefix);

	ImGui::Text("Sprite Name");
	ImGui::SameLine(100.f);

	bool valid = !m_SelectedTileIndices.empty() && !m_Sprites.empty();

	string name{};
	if (valid) name = WStrToStr(m_Sprites[m_SelectedTileIndices.front()].second->GetName());

	if (ImGui::InputText("##SPRITE_NAME", &name))
		if (valid) m_Sprites[m_SelectedTileIndices.front()].second->SetName(StrToWStr(name));

	ImGui::Text("UV Offset");
	ImGui::SameLine(100.f);

	Vec2 offset{};
	if (valid) offset = m_Sprites[m_SelectedTileIndices.front()].second->GetOffsetUV();
	if (ImGui::DragFloat2("##UV_OFFSET", offset, 0.001f)) {
		if (valid) m_Sprites[m_SelectedTileIndices.front()].second->SetOffsetUV(offset);
	}

	ImGui::Dummy(ImVec2(0.f, 20.f));

	// 오른쪽으로 커서 이동
	EditorMgr::RightAlignNextItem({ "Except", "Include" });

	if (ImGui::Button("Except"))
		if (valid) {
			for (size_t i = 0, end = m_SelectedTileIndices.size(); i < end; ++i)
				m_Sprites[m_SelectedTileIndices[i]].first = false;
		}
		
	ImGui::SameLine();

	if (ImGui::Button("Include"))
		if (valid) {
			for (size_t i = 0, end = m_SelectedTileIndices.size(); i < end; ++i)
				m_Sprites[m_SelectedTileIndices[i]].first = true;
		}
}

void AtlasEditorUI::ControlButtons() {
	if (ImGui::Button("Clear")) {
		m_Texture = nullptr;
		m_Sprites.clear();
		m_Grid[0] = m_Grid[1] = 1;
		m_SelectedTileIndices.clear();
	}
	ImGui::SameLine();

	EditorMgr::RightAlignNextItem({ "Apply", "Save" });

	if (ImGui::Button("Save")) SaveSprites(); 
	ImGui::SameLine();
	if (ImGui::Button("Apply")) MakeSprites();
}

void AtlasEditorUI::LowerPanel() {
	const float spacing = ImGui::GetStyle().ItemSpacing.x;

	int count = 0;
	for (size_t i = 0, end = m_Sprites.size(); i < end; ++i)
		if (m_Sprites[i].first) ++count;

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

	for (int i = 0, cnt = 1, end = static_cast<int>(m_Sprites.size()); i < end; ++i) {
		auto sprite = m_Sprites[i];
		if (!sprite.first) continue;

		if (i != 0) ImGui::SameLine();

		auto leftTopUV = sprite.second->GetLeftTopUV();
		auto sliceUV = sprite.second->GetSliceUV();
		auto backgroundUV = sprite.second->GetBackgroundUV();
		auto offsetUV = sprite.second->GetOffsetUV();

		auto actualLeftTopUV = leftTopUV + sliceUV * 0.5f - backgroundUV * 0.5f;
		actualLeftTopUV += offsetUV;

		// 인덱스 텍스트
		{
			ImGui::Text(format("{}.", cnt).c_str());
			ImGui::SameLine();
		}

		auto originPos = ImGui::GetCursorScreenPos();

		// 스프라이트 이미지
		{
			ImGui::ImageWithBg(
				(ImTextureRef)sprite.second->GetAtlas()->GetSRV().Get()
				, ImVec2(SpriteImageSize, SpriteImageSize)
				, actualLeftTopUV, actualLeftTopUV + backgroundUV
				, ImVec4(0.f, 0.f, 0.f, 1.f));
		}

		ImGui::SetCursorScreenPos(originPos);

		// 이미지 위의 선택 영역
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			bool selected = IsTileSelected(i);
			if (ImGui::Selectable(format("##TILE_{}", i).c_str()
				, &selected
				, ImGuiSelectableFlags_None
				, ImVec2(SpriteImageSize, SpriteImageSize)))
				SelectTile(i);

			ImGui::PopStyleVar();
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(10.f, 0.f));

		++cnt;
	}

	ImGui::EndChild();
}

void AtlasEditorUI::MakeSprites() {
	if (m_Texture == nullptr) return;

	m_Sprites.clear();

	auto rows = m_Grid[0];
	auto cols = m_Grid[1];

	auto slice = GetSlice();

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			Ptr<ASprite> sprite = NEW ASprite();
			sprite->SetAtlas(m_Texture);

			auto leftTop = Vec2(slice.x * col, 0.f) + Vec2(0.f, slice.y * row);

			auto idx = row * cols + col;

			sprite->SetName(format(L"{}_{}", StrToWStr(m_Prefix), idx));

			sprite->SetSliceUV(slice);
			sprite->SetLeftTopUV(leftTop);
			sprite->SetBackgroundUV(m_BackgroundUV);

			m_Sprites.push_back({ true, sprite });
		}
	}
}

void AtlasEditorUI::SaveSprites() {
	wstring contentPath = CONTENT_PATH;

	for (const auto& sprite : m_Sprites) 
		if (sprite.first) 
			sprite.second->Save(
				contentPath + format(L"Sprite\\{}.sprite", sprite.second->GetName()));
}

bool AtlasEditorUI::IsTileSelected(int index) {
	if (m_SelectedTileIndices.empty()) return false;

	for (size_t i = 0, end = m_SelectedTileIndices.size(); i < end; ++i)
		if (m_SelectedTileIndices[i] == index) return true;

	return false;
}

void AtlasEditorUI::SelectTile(int index) {
	if (!ImGui::GetIO().KeyShift) m_SelectedTileIndices.clear();

	auto begin = m_SelectedTileIndices.begin();
	auto end = m_SelectedTileIndices.end();

	auto iter = find(begin ,end , index);
	if (iter != end) {
		iter_swap(iter, end - 1);
		m_SelectedTileIndices.pop_back();
	}
	else {
		m_SelectedTileIndices.push_back(index);
	}
}

Vec2 AtlasEditorUI::GetSlice() {
	if (m_Texture == nullptr) return Vec2(0.f);

	auto texWidth = m_Texture->GetWidth();
	auto texHeight = m_Texture->GetHeight();

	auto spriteSizeX = texWidth / static_cast<float>(m_Grid[1]);
	auto spriteSizeY = texHeight / static_cast<float>(m_Grid[0]);

	auto slice = Vec2(spriteSizeX / texWidth, spriteSizeY / texHeight);

	return slice;
}