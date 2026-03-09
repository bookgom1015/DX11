#include "pch.h"
#include "BillboardRenderUI.h"

BillboardRenderUI::BillboardRenderUI()
	: ComponentUI(EComponent::E_BillboardRender, "BillboardRenderUI") {
}

BillboardRenderUI::~BillboardRenderUI() {}

void BillboardRenderUI::Tick_UI() {
	OutputTitle("BillboardRender");
}