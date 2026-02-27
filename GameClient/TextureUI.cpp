#include "pch.h"
#include "TextureUI.h"

#include "ATexture.h"

TextureUI::TextureUI()
	: AssetUI(ASSET_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::Tick_UI()
{
	OutputTitle();

	Ptr<ATexture> pTexture = (ATexture*)GetTargetAsset().Get();

	{
		ImGui::Text("Texture");
		ImGui::SameLine(100);

		// 이미지 샘플		
		ImGui::ImageWithBg((ImTextureRef)pTexture->GetSRV().Get()
			, ImVec2(200, 200)
			, Vec2(0.f, 0.f), Vec2(1.f, 1.f)
			, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	{
		// 해상도
		int Width = pTexture->GetWidth();
		int Height = pTexture->GetHeight();

		ImGui::Text("Resolution");
		ImGui::SameLine(100);

		// 현재 줄에서 남은 가용 영역
		float avail = ImGui::GetContentRegionAvail().x;

		// 간격 고려
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float half = (avail - spacing) * 0.5f;

		ImGui::SetNextItemWidth(half);
		ImGui::InputInt("##Width", &Width, 0, 0, ImGuiInputTextFlags_ReadOnly);

		ImGui::SameLine();

		ImGui::SetNextItemWidth(half);
		ImGui::InputInt("##Height", &Height, 0, 0, ImGuiInputTextFlags_ReadOnly);
	}
}