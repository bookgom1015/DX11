#include "pch.h"
#include "SceneUI.h"

#include "Device.h"
#include "KeyMgr.h"

SceneUI::SceneUI() : EditorUI("Scene") {}

SceneUI::~SceneUI() {}

void SceneUI::Tick_UI() {
	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	m_SceneHWND = (HWND)viewport->PlatformHandle;

	ImVec2 avail = ImGui::GetContentRegionAvail();   // 지금 커서 위치에서 남은 영역
	if (avail.x < 1.f) avail.x = 1.f;
	if (avail.y < 1.f) avail.y = 1.f;

	m_SceneSize = Vec2(avail.x, avail.y);

	auto srv = Device::GetInst()->GetScene_Srv();
	ImGui::Image((ImTextureID)srv, avail);

	// 방금 그린 위젯 기준
	if (ImGui::IsItemHovered())	{
		ImVec2 mouseScreen = ImGui::GetMousePos();
		ImVec2 itemMin = ImGui::GetItemRectMin();

		ImVec2 localPos;
		localPos.x = mouseScreen.x - itemMin.x;
		localPos.y = mouseScreen.y - itemMin.y;

		auto vec = KeyMgr::GetInst()->GetMousePos();

		KeyMgr::GetInst()->SetMousePosOnScene({ localPos.x, localPos.y });
		
		//ImGui::Text("Local Mouse: %.1f, %.1f", localPos.x, localPos.y);
		//ImGui::SameLine();
		//ImGui::Text("Mouse: %.1f, %.1f", vec.x, vec.y);
	}
}