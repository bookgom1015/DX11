#include "pch.h"
#include "LevelUI.h"

LevelUI::LevelUI() : AssetUI(EAsset::E_Level) {}

LevelUI::~LevelUI() {}

void LevelUI::Tick_UI() {
	OutputTitle();
}