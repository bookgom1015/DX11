#include "pch.h"
#include "Menu.h"

#include "Engine.h"

#include "AssetMgr.h"
#include "EditorMgr.h"
#include "RenderMgr.h"

#include "ContentUI.h"

namespace {
	void HandleBorderlessWindowDragFromMenuBar(HWND hwnd) {
		// 메뉴바 윈도우 기준 좌표
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();

		ImVec2 mouse = ImGui::GetMousePos();

		bool hovered =
			mouse.x >= pos.x &&
			mouse.x <= pos.x + size.x &&
			mouse.y >= pos.y &&
			mouse.y <= pos.y + size.y;

		bool anyPopupOpen = ImGui::IsPopupOpen(nullptr,
			ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);

		// 메뉴 항목 위가 아니라 "빈 공간"에서만 이동
		bool hoveringItem = ImGui::IsAnyItemHovered();

		if (!anyPopupOpen && hovered && !hoveringItem &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			::ReleaseCapture();
			::SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
	}
}

Menu::Menu() : EditorUI("Menu") {}

Menu::~Menu() {}

void Menu::Tick_UI() {}

void Menu::Tick() {
	if (ImGui::BeginMainMenuBar()) {
		File();		
		View();
		GameObject();
		Asset();
		Render();

		bool emptySpaceHovered = ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered();
		if (emptySpaceHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			ReleaseCapture();
			SendMessage(Engine::GetInst()->GetMainWndHwnd(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}

		CloseButton();

		ImGui::EndMainMenuBar();
	}
}

void Menu::File() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Level Save")) {}

		if (ImGui::MenuItem("Level Load")) {}

		if (ImGui::MenuItem("Exit")) {
			PostQuitMessage(0);
		}

		ImGui::EndMenu();
	}
}

void Menu::View() {
	if (ImGui::BeginMenu("View")) {
		bool ShowDemo = EditorMgr::GetInst()->IsShowDemo();
		if (ImGui::MenuItem("Demo", nullptr, &ShowDemo, true))
			EditorMgr::GetInst()->ShowDemo(ShowDemo);

		Ptr<EditorUI> pScene = EditorMgr::GetInst()->FindUI("Scene");
		bool SceneActive = pScene->IsActive();
		if (ImGui::MenuItem("Scene", nullptr, &SceneActive))
			pScene->SetActive(SceneActive);

		Ptr<EditorUI> pInspector = EditorMgr::GetInst()->FindUI("Inspector");
		bool InspectorActive = pInspector->IsActive();
		if (ImGui::MenuItem("Inspector", nullptr, &InspectorActive))
			pInspector->SetActive(InspectorActive);

		Ptr<EditorUI> pOutliner = EditorMgr::GetInst()->FindUI("Outliner");
		bool OutlinerActive = pOutliner->IsActive();
		if (ImGui::MenuItem("Outliner", nullptr, &OutlinerActive))
			pOutliner->SetActive(OutlinerActive);

		Ptr<EditorUI> pContent = EditorMgr::GetInst()->FindUI("Content");
		bool ContentActive = pContent->IsActive();
		if (ImGui::MenuItem("Content", nullptr, &ContentActive))
			pContent->SetActive(ContentActive);

		Ptr<EditorUI> pLog = EditorMgr::GetInst()->FindUI("Log");
		bool LogActive = pLog->IsActive();
		if (ImGui::MenuItem("Log", nullptr, &LogActive))
			pLog->SetActive(LogActive);

		ImGui::EndMenu();
	}
}

void Menu::GameObject() {
	if (ImGui::BeginMenu("GameObject"))	
		ImGui::EndMenu();
}

void Menu::Asset() {
	if (ImGui::BeginMenu("Asset")) {
		if (ImGui::BeginMenu("Create Asset")) {
			if (ImGui::MenuItem("Create Material")) {
				Ptr<AMaterial> pMtrl = NEW AMaterial;
				wstring Key = GetAssetName(EAsset::E_Material, L"Material\\Default Material");
				AssetMgr::GetInst()->AddAsset(Key, pMtrl.Get());				
			}

			if (ImGui::MenuItem("Create Sprite")) {

			}

			if (ImGui::MenuItem("Create Flipbook")) {

			}

			if (ImGui::MenuItem("Create TileMap")) {

			}

			ImGui::EndMenu();
		}	

		ImGui::EndMenu();
	}
}

void Menu::Render() {
	if (ImGui::BeginMenu("Render")) {
		ImGui::MenuItem("Show Collider", NULL, &RenderMgr::DebugRender);

		if (ImGui::BeginMenu("Gamma Correction")) {
			ImGui::MenuItem("Enabled", NULL, &RenderMgr::GammaEnabled);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tone Mapping")) {
			ImGui::MenuItem("Enabled", NULL, &RenderMgr::ToneEnabled);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Combo(
				"##Type",
				reinterpret_cast<int*>(&RenderMgr::ToneType),
				EToneMapper::TypeNames,
				EToneMapper::Count)) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Bloom")) {
			ImGui::MenuItem("Enabled", NULL, &RenderMgr::BloomEnabled);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Pixel")) {
			ImGui::MenuItem("Enabled", NULL, &RenderMgr::PixelEnabled);

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

wstring Menu::GetAssetName(EAsset::Type _Type, const wstring& _Name) {
	wstring Ext{};

	switch (_Type) {
	case EAsset::E_Mesh:
		Ext = L".mesh";
		break;
	case EAsset::E_Material:
		Ext = L".mtrl";
		break;
	case EAsset::E_Texture:
	case EAsset::E_Sound:
	case EAsset::E_GraphicShader:
	case EAsset::E_ComputeShader:
		assert(nullptr);
		break;
	case EAsset::E_Sprite:
		Ext = L".sprite";
		break;
	case EAsset::E_Flipbook:
		Ext = L".flip";
		break;
	case EAsset::E_Prefab:
		Ext = L".pref";
		break;
	case EAsset::E_Level:
		Ext = L".lv";
		break;
	}

	int i = 0;
	while (true) {
		wchar_t Num[50] = {};
		swprintf_s(Num, 50, L"_%d", i);

		wstring AssetName = wstring(_Name + Num + Ext);
		if (AssetMgr::GetInst()->FindAsset(_Type, AssetName) == nullptr)
			return AssetName;

		i++;
	}
}

void Menu::CloseButton() {
	// 메뉴바 높이(버튼 높이)
	const float bar_h = ImGui::GetFrameHeight();

	// 아이콘처럼 보이게: "X" 버튼 폭은 높이 정도로 맞추면 자연스러움
	const float btn_w = bar_h;

	// 현재 메뉴바 윈도우의 오른쪽 끝으로 커서를 이동
	float right = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX(right - btn_w);

	// 스타일 조금: 패딩 줄여서 꽉 찬 느낌
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	// (선택) hover 시 색 강조를 원하면 PushStyleColor로 커스텀 가능

	if (ImGui::Button("X", ImVec2(btn_w, bar_h))) {
		// 가장 안전: 메시지로 닫기(윈도우 스레드가 처리)
		::PostMessage(Engine::GetInst()->GetMainWndHwnd(), WM_CLOSE, 0, 0);
	}

	ImGui::PopStyleVar();
}