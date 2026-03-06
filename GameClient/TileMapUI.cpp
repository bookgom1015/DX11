#include "pch.h"
#include "TileMapUI.h"


TileMapUI::TileMapUI()	: AssetUI(EAsset::E_TileMap) {}

TileMapUI::~TileMapUI() {}

void TileMapUI::Tick_UI() {
	OutputTitle();
}