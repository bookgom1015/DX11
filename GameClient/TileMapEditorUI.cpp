#include "pch.h"
#include "TileMapEditorUI.h"

#include "EditorMgr.h"

TileMapEditorUI::TileMapEditorUI() 
	: EditorUI("TileMap Editor")
	, m_Grid{ 1, 1 }
	, m_BufGrid{ 1,1 } {}

TileMapEditorUI::~TileMapEditorUI() {}

void TileMapEditorUI::Tick_UI() {
	auto avail = ImGui::GetContentRegionAvail();
	auto size = ImGui::GetStyle().ScrollbarSize;

	if (ImGui::BeginTable("TileMapEditorTable", 2)) {
		ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthStretch, 8.f);
		ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthStretch, 2.f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		LeftPanel();

		ImGui::TableSetColumnIndex(1);
		RightPanel();

		ImGui::EndTable();
	}
}

void TileMapEditorUI::LeftPanel() {
	auto leftTopPos = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

	auto avail = ImGui::GetContentRegionAvail();
	ImGui::ImageWithBg(nullptr, avail, Vec2(0.f), Vec2(1.f), Vec4(0.f, 0.f, 0.f, 1.f));

	auto imageRectSize = Vec2(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
	auto imageMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
	auto imageMax = Vec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

	Grid(imageMin, imageMax);
	Sprite(leftTopPos, imageRectSize);
	Selectable(leftTopPos, imageRectSize);
}

void TileMapEditorUI::Grid(Vec2 imageMin, Vec2 imageMax) {
	const ImU32 innerColor = IM_COL32(0, 255, 0, 255);
	const ImU32 outerColor = IM_COL32(0, 0, 0, 200);
	const float innerThick = 1.f;
	const float outerThick = innerThick * 4.f + 1.f;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(imageMin, imageMax + Vec2(outerThick), true);

	int gridCols = m_Grid[1];
	int gridRows = m_Grid[0];

	auto imageSize = imageMax - imageMin;
	const float cellW = imageSize.x / static_cast<float>(gridCols);
	const float cellH = imageSize.y / static_cast<float>(gridRows);

	// 세로선
	for (int x = 0; x <= gridCols; ++x) {
		float px = imageMin.x + x * cellW;
		dl->AddLine(
			Vec2(px, imageMin.y),
			Vec2(px, imageMax.y),
			outerColor,
			outerThick
		);
	}

	// 가로선
	for (int y = 0; y <= gridRows; ++y) {
		float py = imageMin.y + y * cellH;
		dl->AddLine(
			Vec2(imageMin.x, py),
			Vec2(imageMax.x, py),
			outerColor,
			outerThick
		);
	}

	// 세로선
	for (int x = 0; x <= gridCols; ++x) {
		float px = imageMin.x + x * cellW;
		dl->AddLine(
			Vec2(px, imageMin.y),
			Vec2(px, imageMax.y),
			innerColor,
			innerThick
		);
	}

	// 가로선
	for (int y = 0; y <= gridRows; ++y) {
		float py = imageMin.y + y * cellH;
		dl->AddLine(
			Vec2(imageMin.x, py),
			Vec2(imageMax.x, py),
			innerColor,
			innerThick
		);
	}

	dl->PopClipRect();
}

void TileMapEditorUI::Sprite(Vec2 leftTopPos, Vec2 imageRectSize) {
	int rows = m_Grid[0];
	int cols = m_Grid[1];

	auto numTiles = rows * cols;
	for (int i = 0; i < numTiles; ++i) {

		auto texWidth = imageRectSize.x;
		auto texHeight = imageRectSize.y;

		auto spriteSizeX = texWidth / static_cast<float>(cols);
		auto spriteSizeY = texHeight / static_cast<float>(rows);

		int row = i / cols;
		int col = i % cols;
		auto newPos = leftTopPos + Vec2(col * spriteSizeX, row * spriteSizeY);
		auto size = Vec2(spriteSizeX, spriteSizeY);

		ImGui::SetCursorScreenPos(newPos);

		Ptr<ASprite> sprite{};
		if (m_TileMap != nullptr) sprite = m_TileMap->GetSprite(
			static_cast<UINT>(row), static_cast<UINT>(col));
		
		ID3D11ShaderResourceView* srv{};
		Vec2 uv0{};
		Vec2 uv1{};
		if (sprite != nullptr) {
			srv = sprite->GetAtlas()->GetSRV().Get();
			uv0 = sprite->GetLeftTopUV();
			uv1 = uv0 + sprite->GetSliceUV();
		}

		ImGui::ImageWithBg(srv, size, uv0, uv1, Vec4(0.f));
	}
}

void TileMapEditorUI::Selectable(Vec2 leftTopPos, Vec2 imageRectSize) {
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vec2(0.f));
	ImGui::BeginGroup();

	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	int rows = m_Grid[0];
	int cols = m_Grid[1];
	int numTiles = rows * cols;

	m_TileRects.clear();
	m_TileRects.reserve(numTiles);
	m_HoveredTiles.clear();

	const float texWidth = imageRectSize.x;
	const float texHeight = imageRectSize.y;

	const float spriteSizeX = texWidth / static_cast<float>(cols);
	const float spriteSizeY = texHeight / static_cast<float>(rows);

	// 전체 그리드 영역
	SimpleRect gridRect(
		Vec2(leftTopPos.x, leftTopPos.y),
		Vec2(leftTopPos.x + texWidth, leftTopPos.y + texHeight));

	// -------------------------
	// 드래그 시작 판정
	// -------------------------
	const bool mouseInGrid =
		(io.MousePos.x >= gridRect.Min.x && io.MousePos.x <= gridRect.Max.x &&
			io.MousePos.y >= gridRect.Min.y && io.MousePos.y <= gridRect.Max.y);

	if (!m_DragSelect.Dragging 
		&& mouseInGrid 
		&& ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		m_DragSelect.Dragging = true;
		m_DragSelect.BoxSelecting = false;
		m_DragSelect.Start = Vec2(io.MousePos.x, io.MousePos.y);
		m_DragSelect.End = Vec2(io.MousePos.x, io.MousePos.y);
	}

	// -------------------------
	// 타일 생성
	// -------------------------
	for (int i = 0; i < numTiles; ++i) {
		int row = i / cols;
		int col = i % cols;

		Vec2 newPos(
			leftTopPos.x + col * spriteSizeX,
			leftTopPos.y + row * spriteSizeY);

		Vec2 size(spriteSizeX, spriteSizeY);

		ImGui::SetCursorScreenPos(newPos);

		// 클릭용 selectable
		bool selected = false;
		if (ImGui::Selectable(
			format("##TILE_{}", i).c_str()
			, &selected
			, ImGuiSelectableFlags_None
			, size)) {
			// 클릭했는데 드래그 아니면 단일 타일 적용
			if (!m_DragSelect.BoxSelecting) {
				if (m_TileMap != nullptr && m_SpriteBrush != nullptr) {
					m_TileMap->SetSprite(
						static_cast<UINT>(row),
						static_cast<UINT>(col),
						m_SpriteBrush);
				}
			}
		}

		m_TileRects.emplace_back(newPos, newPos + size);
	}

	// -------------------------
	// 드래그 중 처리
	// -------------------------
	if (m_DragSelect.Dragging) {
		m_DragSelect.End = Vec2(io.MousePos.x, io.MousePos.y);

		const float dx = m_DragSelect.End.x - m_DragSelect.Start.x;
		const float dy = m_DragSelect.End.y - m_DragSelect.Start.y;
		const float dragDistSq = dx * dx + dy * dy;

		// 살짝 움직인 건 클릭 취급, 어느 정도 이상 움직여야 박스 선택
		if (dragDistSq > 9.0f)
			m_DragSelect.BoxSelecting = true;

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
			if (m_DragSelect.BoxSelecting 
				&& m_TileMap != nullptr 
				&& m_SpriteBrush != nullptr) {
				for (int tileIndex : m_HoveredTiles) {
					int row = tileIndex / cols;
					int col = tileIndex % cols;

					m_TileMap->SetSprite(
						static_cast<UINT>(row),
						static_cast<UINT>(col),
						m_SpriteBrush);
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

void TileMapEditorUI::RightPanel() {
	GridControl();

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	BrushControl();

	ImGui::Dummy(ImVec2(0.f, 20.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 20.f));

	ControlButtons();
}

void TileMapEditorUI::GridControl() {
	// 그리드 크기
	{
		ImGui::Text("Grid Size");
		ImGui::SameLine(100.f);

		ImGui::DragInt2("##GRID_SIZE", m_BufGrid, 0.05f, 1, 64);
	}
}

void TileMapEditorUI::BrushControl() {
	{
		ImGui::Text("Atlas");
		ImGui::SameLine(100.f);

		string atlas{};
		if (m_TileMap != nullptr && m_TileMap->GetAtlas() != nullptr)
			atlas = WStrToStr(m_TileMap->GetAtlas()->GetKey());
		ImGui::InputText("#ATLAS_KEY", &atlas, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget()) {
			if (m_TileMap != nullptr) {
				EditorMgr::GetInst()->AcceptAssetDragDrop("Content", EAsset::E_Texture, [&](Ptr<Asset> asset) {
					auto texture = static_cast<ATexture*>(asset.Get());
					m_TileMap->SetAtlas(texture);
					m_SpriteBrush = nullptr;
				});
			}

			ImGui::EndDragDropTarget();
		}
	}
	{
		ImGui::Text("Sprite");
		ImGui::SameLine(100.f);

		string sprite{};
		if (m_TileMap != nullptr && m_SpriteBrush != nullptr)
			sprite = WStrToStr(m_SpriteBrush->GetKey());
		ImGui::InputText("#SPRITE_KEY", &sprite, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget()) {
			if (m_TileMap != nullptr) {
				EditorMgr::GetInst()->AcceptAssetDragDrop("Content", EAsset::E_Sprite, [&](Ptr<Asset> asset) {
					auto sprite = static_cast<ASprite*>(asset.Get());
					if (m_TileMap->GetAtlas() != sprite->GetAtlas()) return;

					m_SpriteBrush = sprite;
				});
			}

			ImGui::EndDragDropTarget();
		}
	}
}

void TileMapEditorUI::ControlButtons() {
	EditorMgr::RightAlignNextItem({ "Save", "Apply" });

	if (ImGui::Button("Save")) {
		Ptr<ATileMap> tileMap = new ATileMap;

		//tileMap->SetAtlas();
	}
	ImGui::SameLine();
	if (ImGui::Button("Apply")) {
		memcpy(m_Grid, m_BufGrid, sizeof(int) * 2);

		m_TileMap = new ATileMap;
		m_TileMap->SetRowCol(m_Grid[0], m_Grid[1]);
	}
}