#include "pch.h"
#include "FlipbookUI.h"

#include "AFlipbook.h"
#include "TimeMgr.h"

FlipbookUI::FlipbookUI()
	: AssetUI(ASSET_TYPE::FLIPBOOK)
{
}

FlipbookUI::~FlipbookUI()
{
}

void FlipbookUI::Tick_UI()
{
	OutputTitle();

	Ptr<AFlipbook> pFlipbook = (AFlipbook*)GetTargetAsset().Get();

	mElapsedTime += E_DT;

	{
		ImGui::Text("Flipbook");
		ImGui::SameLine(100);

		auto numSprites = pFlipbook->GetSpriteCount();
		auto frameTime = 1.f / mFPS;

		if (mElapsedTime >= frameTime) {
			mElapsedTime -= frameTime;
			++mIndex;
			if (mIndex == numSprites) mIndex = 0;
		}

		auto sprite = pFlipbook->GetSprite(mIndex);

		Vec2 leftTopUV = sprite->GetLeftTopUV();
		Vec2 sliceUV = sprite->GetSliceUV();

		auto atlas = sprite->GetAtlas();

		ImGui::ImageWithBg((ImTextureRef)atlas->GetSRV().Get()
			, ImVec2(200, 200)
			, leftTopUV, leftTopUV + sliceUV
			, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	{
		ImGui::Text("FPS");
		ImGui::SameLine(100);
		ImGui::SliderFloat("##FPS", &mFPS, 1.f, 120.f);
	}
}