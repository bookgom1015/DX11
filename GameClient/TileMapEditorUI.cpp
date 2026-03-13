#include "pch.h"
#include "TileMapEditorUI.h"

#include "EditorMgr.h"
#include "PathMgr.h"

TileMapEditorUI::TileMapEditorUI() 
	: EditorUI("TileMap Editor")
	, m_Grid{ 1, 1 }
	, m_BufGrid{ 1,1 }
	, m_BrushSize{ 1.f } {
	m_TileRects.reserve(1);
}

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
	auto leftTopPos = EditorMgr::GetCursorScreenPos();

	auto avail = ImGui::GetContentRegionAvail();
	ImGui::ImageWithBg(nullptr, avail, Vec2(0.f), Vec2(1.f), Vec4(0.f, 0.f, 0.f, 1.f));

	auto imageRectSize = EditorMgr::GetItemRectSize();
	auto imageMin = EditorMgr::GetItemRectMin();
	auto imageMax = EditorMgr::GetItemRectMax();

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

	Vec2 spriteSize = Vec2(
		imageRectSize.x / static_cast<float>(cols),
		imageRectSize.y / static_cast<float>(rows));

	auto numTiles = rows * cols;
	for (int i = 0; i < numTiles; ++i) {

		int row = i / cols;
		int col = i % cols;
		auto newPos = leftTopPos + Vec2(col * spriteSize.x, row * spriteSize.y);

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

		ImGui::ImageWithBg(srv, spriteSize, uv0, uv1, Vec4(0.f));
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

	const Vec2 spriteSize = Vec2(
		imageRectSize.x / static_cast<float>(cols),
		imageRectSize.y / static_cast<float>(rows));	

	// 전체 그리드 영역
	SimpleRect gridRect(
		Vec2(leftTopPos.x, leftTopPos.y),
		Vec2(leftTopPos.x + imageRectSize.x, leftTopPos.y + imageRectSize.y));

	const Vec2 mousePos = Vec2(io.MousePos.x, io.MousePos.y);

	SimpleRect rect{
		.Min = Vec2(mousePos.x - m_BrushSize, mousePos.y - m_BrushSize),
		.Max = Vec2(mousePos.x + m_BrushSize, mousePos.y + m_BrushSize)	};

	// -------------------------
	// 드래그 시작 판정
	// -------------------------
	const bool mouseInGrid = Util::MouseInRect(mousePos, gridRect.Min, gridRect.Max);
	const bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left)
		|| ImGui::IsMouseClicked(ImGuiMouseButton_Right);
	if (!m_DragSelect.Dragging  && mouseInGrid  && clicked) {
		m_DragSelect = {
			.Dragging = true,
			.BoxSelecting = true,
			.RightClick = ImGui::IsMouseClicked(ImGuiMouseButton_Right),
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
			// 겹치는 타일 모으기
			for (int i = 0, end = static_cast<int>(m_TileRects.size()); i < end; ++i) {
				if (Util::Intersects(rect, m_TileRects[i])) {
					int row = i / cols;
					int col = i % cols;

					if (m_TileMap != nullptr && m_SpriteBrush != nullptr) {
						if (m_DragSelect.RightClick) {
							m_TileMap->ResetSprite(
								static_cast<UINT>(row),
								static_cast<UINT>(col));
						}
						else {
							m_TileMap->SetSprite(
								static_cast<UINT>(row),
								static_cast<UINT>(col),
								m_SpriteBrush);
						}
					}
				}
			}

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
			m_DragSelect.Dragging = false;
			m_DragSelect.BoxSelecting = false;
			m_HoveredTiles.clear();
		}
	}

	// 브러쉬 시각화
	drawList->AddRectFilled(
		rect.Min, rect.Max,
		IM_COL32(80, 140, 255, 40));

	drawList->AddRect(
		rect.Min, rect.Max,
		IM_COL32(80, 140, 255, 200));

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

	TileMapControl();

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
	{
		ImGui::Text("Brush Size");
		ImGui::SameLine(100.f);

		ImGui::DragFloat("##BRUSH_SIZE", &m_BrushSize, 0.1f, 1.f, 128.f);
	}
}

void TileMapEditorUI::TileMapControl() {
	{
		ImGui::Text("Name");
		ImGui::SameLine(100.f);

		string name{};
		if (m_TileMap != nullptr) name = WStrToStr(m_TileMap->GetName());

		ImGui::SetNextItemWidth(200.f);
		if (ImGui::InputText("##TIMEMAP_NAME", &name)) 
			if (m_TileMap != nullptr) m_TileMap->SetName(StrToWStr(name));
	}
}

void TileMapEditorUI::ControlButtons() {
	EditorMgr::RightAlignNextItem({ "Clear", "Save", "Apply" });

	if (ImGui::Button("Clear")) {
		if (m_TileMap != nullptr) {
			for (int row = 0; row < m_Grid[0]; ++row) 
				for (int col = 0; col < m_Grid[1]; ++col) 
					m_TileMap->ResetSprite(row, col);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		if (m_TileMap != nullptr) {
			m_TileMap->Save(format(
				L"{}TileMap\\{}.tile", CONTENT_PATH, m_TileMap->GetName()));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Apply")) {
		memcpy(m_Grid, m_BufGrid, sizeof(int) * 2);

		m_TileMap = new ATileMap;
		m_TileMap->SetRowCol(m_Grid[0], m_Grid[1]);
	}
}