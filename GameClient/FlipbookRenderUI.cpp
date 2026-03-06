#include "pch.h"
#include "FlipbookRenderUI.h"

FlipbookRenderUI::FlipbookRenderUI()
	: ComponentUI(EComponent::E_FlipbookRender, "FlipbookRenderUI") {}

FlipbookRenderUI::~FlipbookRenderUI() {}

void FlipbookRenderUI::Tick_UI() {
	OutputTitle("FlipbookRender");
}
