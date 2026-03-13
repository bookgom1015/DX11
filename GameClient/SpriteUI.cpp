#include "pch.h"
#include "SpriteUI.h"

#include "EditorMgr.h"

#include "ASprite.h"

SpriteUI::SpriteUI() : AssetUI(EAsset::E_Sprite) {}

SpriteUI::~SpriteUI() {}

void SpriteUI::Tick_UI() {
	OutputTitle();

	Ptr<ASprite> pSprite = (ASprite*)GetTargetAsset().Get();

	string Key = string(pSprite->GetKey().begin(), pSprite->GetKey().end());

	auto leftTopUV = pSprite->GetLeftTopUV();
	auto sliceUV = pSprite->GetSliceUV();
	auto backgroundUV = pSprite->GetBackgroundUV();
	auto offsetUV = pSprite->GetOffsetUV();

	auto actualLeftTopUV = leftTopUV + sliceUV * 0.5f - backgroundUV * 0.5f;
	actualLeftTopUV += offsetUV;

	ImGui::Text("Sprite");
	ImGui::SameLine(100.f);
	ImGui::ImageWithBg(
		(ImTextureRef)pSprite->GetAtlas()->GetSRV().Get()
		, ImVec2(200.f, 200.f)
		, actualLeftTopUV, actualLeftTopUV + backgroundUV
		, ImVec4(0.f, 0.f, 0.f, 1.f));

	ImGui::Text("Name");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::InputText("##SpriteName", Key.data(), Key.length() + 1, ImGuiInputTextFlags_ReadOnly);

	ImGui::Dummy(Vec2(0.f, 10.f));
	ImGui::Separator();
	ImGui::Dummy(Vec2(0.f, 10.f));

	// Sprite 에 Atlas 텍스쳐가 세팅되어있는지 확인
	Ptr<ATexture> pAtlas = pSprite->GetAtlas();
	bool IsAtlas = pAtlas.Get();

	ImGui::Text("Atlas");
	ImGui::SameLine(100.f);
	ImGui::ImageWithBg(
		pAtlas->GetSRV().Get()
		, Vec2(200.f)
		, Vec2(0.f), Vec2(1.f)
		, Vec4(0.f, 0.f, 0.f, 1.f));

	// Atlas 이름
	string AtlasName{};
	if (IsAtlas) AtlasName = WStrToStr(pAtlas->GetKey());

	ImGui::Text("Reference");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::InputText("##AtlasName", &AtlasName, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget()) {
		EditorMgr::AcceptAssetDragDrop("Content", EAsset::E_Texture, [&](Ptr<Asset> asset) {
			auto texture = static_cast<ATexture*>(asset.Get());
			pSprite->SetAtlas(texture);
		});

		ImGui::EndDragDropTarget();
	}

	// Sprite UV 정보
	Vec2 LeftTopUV = pSprite->GetLeftTopUV();
	Vec2 SliceUV = pSprite->GetSliceUV();
	Vec2 BackgroundUV = pSprite->GetBackgroundUV();
	Vec2 OffsetUV = pSprite->GetOffsetUV();

	ImGui::Text("LeftTop");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::BeginDisabled(!IsAtlas);
	{
		if (IsAtlas) {
			int pixel[2] = { LeftTopUV.x * (float)pAtlas->GetWidth()
						  ,  LeftTopUV.y * (float)pAtlas->GetHeight() };

			if (ImGui::InputInt2("##LeftTop", pixel)) {
				pSprite->SetLeftTopUV(Vec2((float)pixel[0] / (float)pAtlas->GetWidth()
					, (float)pixel[1] / (float)pAtlas->GetHeight()));
			}
		}
		else {
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##LeftTop", pixel);
		}
	}
	ImGui::EndDisabled();

	ImGui::Text("Slice");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::BeginDisabled(!IsAtlas);
	{
		if (IsAtlas) {
			int pixel[2] = { SliceUV.x * (float)pAtlas->GetWidth()
						  ,  SliceUV.y * (float)pAtlas->GetHeight() };

			if (ImGui::InputInt2("##Slice", pixel)) {
				pSprite->SetSliceUV(Vec2((float)pixel[0] / (float)pAtlas->GetWidth()
					, (float)pixel[1] / (float)pAtlas->GetHeight()));
			}
		}
		else {
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Slice", pixel);
		}
	}
	ImGui::EndDisabled();


	ImGui::Text("Background");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::BeginDisabled(!IsAtlas);
	{
		if (IsAtlas) {
			int pixel[2] = { BackgroundUV.x * (float)pAtlas->GetWidth()
						  ,  BackgroundUV.y * (float)pAtlas->GetHeight() };

			if (ImGui::InputInt2("##Background", pixel)) {
				pSprite->SetBackgroundUV(Vec2((float)pixel[0] / (float)pAtlas->GetWidth()
					, (float)pixel[1] / (float)pAtlas->GetHeight()));
			}
		}
		else {
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Background", pixel);
		}
	}
	ImGui::EndDisabled();

	ImGui::Text("Offset");
	ImGui::SameLine(100.f);
	ImGui::SetNextItemWidth(200.f);
	ImGui::BeginDisabled(!IsAtlas);
	{
		if (IsAtlas) {
			int pixel[2] = { OffsetUV.x * (float)pAtlas->GetWidth()
						  ,  OffsetUV.y * (float)pAtlas->GetHeight() };

			if (ImGui::InputInt2("##Offset", pixel)) {
				pSprite->SetOffsetUV(Vec2((float)pixel[0] / (float)pAtlas->GetWidth()
					, (float)pixel[1] / (float)pAtlas->GetHeight()));
			}
		}
		else {
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Offset", pixel);
		}
	}
	ImGui::EndDisabled();
}
