#include "pch.h"
#include "GraphicShaderUI.h"

GraphicShaderUI::GraphicShaderUI() : AssetUI(EAsset::E_GraphicShader) {}

GraphicShaderUI::~GraphicShaderUI() {}

void GraphicShaderUI::Tick_UI() {
	OutputTitle();
}