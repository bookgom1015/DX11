#include "pch.h"
#include "ComputeShaderUI.h"

ComputeShaderUI::ComputeShaderUI() : AssetUI(EAsset::E_ComputeShader) {}

ComputeShaderUI::~ComputeShaderUI() {}

void ComputeShaderUI::Tick_UI() {
	OutputTitle();
}