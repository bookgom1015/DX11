#include "pch.h"
#include "TileRenderUI.h"

TileRenderUI::TileRenderUI() 
	: ComponentUI(EComponent::E_TileRender, "TileRenderUI") {}

TileRenderUI::~TileRenderUI() {}


void TileRenderUI::Tick_UI() {
	OutputTitle("TileRender");
}