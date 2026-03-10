#include "pch.h"
#include "Inspector.h"

#include "TransformUI.h"
#include "CameraUI.h"
#include "Collider2DUI.h"
#include "Light2DUI.h"
#include "MeshRenderUI.h"
#include "BillboardRenderUI.h"
#include "SpriteRenderUI.h"
#include "FlipbookRenderUI.h"
#include "TileRenderUI.h"
#include "RigidbodyUI.h"
#include "MeshUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "SoundUI.h"
#include "GraphicShaderUI.h"
#include "ComputeShaderUI.h"
#include "LevelUI.h"
#include "SpriteUI.h"
#include "FlipbookUI.h"
#include "TileMapUI.h"
#include "PrefabUI.h"
#include "AddComponentButton.h"

#define ADD_COMPONENT_UI(ComponentType, type, Size) m_arrComUI[(UINT)ComponentType] = NEW type;\
													m_arrComUI[(UINT)ComponentType]->SetSizeAsChild(Size);\
													AddChildUI(m_arrComUI[(UINT)ComponentType].Get());

#define ADD_ASSET_UI(AssetType, type) m_arrAssetUI[(UINT)AssetType] = NEW type;\
									  AddChildUI(m_arrAssetUI[(UINT)AssetType].Get());

#define ADD_SCRIPT_UI(ScriptType) m_arrScriptUI[(UINT)ScriptType] = NEW ScriptUI(ScriptType);	\
										AddChildUI(m_arrScriptUI[ScriptType].Get());

void Inspector::CreateChildUI() {
	ADD_COMPONENT_UI(EComponent::E_Transform,		TransformUI,		Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_Collider2D,		Collider2DUI,		Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_Camera,			CameraUI,			Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_Light2D,			Light2DUI,			Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_MeshRender,		MeshRenderUI,		Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_BillboardRender, BillboardRenderUI,	Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_SpriteRender,	SpriteRenderUI,		Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_FlipbookRender,	FlipbookRenderUI,	Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_TileRender,		TileRenderUI,		Vec2(0.f, 50.f));
	ADD_COMPONENT_UI(EComponent::E_Rigidbody,		RigidbodyUI,		Vec2(0.f, 50.f));

	for (UINT i = 0; i < SCRIPT_TYPE::Count; ++i) {
		ADD_SCRIPT_UI((SCRIPT_TYPE)i);
	}

	m_AddCompBtn = NEW AddComponentButton;
	AddChildUI(m_AddCompBtn.Get());

	ADD_ASSET_UI(EAsset::E_Mesh, MeshUI);
	ADD_ASSET_UI(EAsset::E_Material, MaterialUI);
	ADD_ASSET_UI(EAsset::E_Texture, TextureUI);
	ADD_ASSET_UI(EAsset::E_Sound, SoundUI);
	ADD_ASSET_UI(EAsset::E_GraphicShader, GraphicShaderUI);
	ADD_ASSET_UI(EAsset::E_ComputeShader, ComputeShaderUI);
	ADD_ASSET_UI(EAsset::E_Level, LevelUI);
	ADD_ASSET_UI(EAsset::E_Sprite, SpriteUI);
	ADD_ASSET_UI(EAsset::E_Flipbook, FlipbookUI);
	ADD_ASSET_UI(EAsset::E_TileMap, TileMapUI);
	ADD_ASSET_UI(EAsset::E_Prefab, PrefabUI);
}