#include "pch.h"
#include "SpriteUI.h"

#include "ASprite.h"

SpriteUI::SpriteUI()
	: AssetUI(ASSET_TYPE::SPRITE)
{
}

SpriteUI::~SpriteUI()
{
}

void SpriteUI::Tick_UI()
{
	OutputTitle();

	Ptr<ASprite> pSprite = (ASprite*)GetTargetAsset().Get();

	{
		ImGui::Text("Sprite");
		ImGui::SameLine(100);

		auto atlas = pSprite->GetAtlas();

		Vec2 leftTopUV = pSprite->GetLeftTopUV();
		Vec2 sliceUV = pSprite->GetSliceUV();

		ImGui::ImageWithBg((ImTextureRef)atlas->GetSRV().Get()
			, ImVec2(200, 200)
			, leftTopUV, leftTopUV + sliceUV
			, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}
