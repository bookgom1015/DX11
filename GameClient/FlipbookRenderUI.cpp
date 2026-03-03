#include "pch.h"
#include "FlipbookRenderUI.h"

void FlipbookRenderUI::Tick_UI()
{
	OutputTitle("FlipbookRender");
}

FlipbookRenderUI::FlipbookRenderUI()
	: ComponentUI(COMPONENT_TYPE::FLIPBOOK_RENDER, "FlipbookRenderUI")
{
}

FlipbookRenderUI::~FlipbookRenderUI()
{
}
