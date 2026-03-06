#include "pch.h"

#include "MeshUI.h"

MeshUI::MeshUI() : AssetUI(EAsset::E_Mesh) {}

MeshUI::~MeshUI() {}

void MeshUI::Tick_UI() {
	OutputTitle();
}
