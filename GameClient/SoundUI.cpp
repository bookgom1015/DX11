#include "pch.h"
#include "SoundUI.h"

SoundUI::SoundUI() : AssetUI(EAsset::E_Sound) {}

SoundUI::~SoundUI() {}

void SoundUI::Tick_UI() {
	OutputTitle();
}