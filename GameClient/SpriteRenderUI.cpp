#include "pch.h"
#include "SpriteRenderUI.h"

SpriteRenderUI::SpriteRenderUI()
	: ComponentUI(EComponent::E_SpriteRender, "SpriteRenderUI") {}

SpriteRenderUI::~SpriteRenderUI() {}

void SpriteRenderUI::Tick_UI() {
	OutputTitle("SpriteRender");
}
