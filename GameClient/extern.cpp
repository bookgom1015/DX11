#include "pch.h"

TransformMatrix g_Trans = {};
GlobalData		g_Global = {};

const char* ASSET_TYPE_STR[EAsset::Count] = {
	"MESH",
	"MATERIAL",
	"TEXTURE",
	"SOUND",
	"GRAPHICSHADER",
	"COMPUTESHADER",
	"LEVEL",
	"SPRITE",
	"FLIPBOOK",
	"TILEMAP",
	"PREFAB"
};

const char* ToString(EAsset::Type _Type) { return ASSET_TYPE_STR[_Type]; }