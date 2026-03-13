#include "pch.h"
#include "AssetMgr.h"

#include "PathMgr.h"
#include "EditorMgr.h"

void AssetMgr::CreateEngineMesh() {
	Ptr<AMesh> pMesh{};

	// ========
	// RectMesh
	// ========
	Vtx arrVtx[4]{};

	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 0.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);
	arrVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 0.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	UINT arrIdx[6] = { 0, 2, 3, 0, 1, 2 };

	// 사각형 메쉬 생성
	pMesh = NEW AMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 6);	
	AddAsset(L"RectMesh", pMesh.Get());

	// ==================
	// RectMesh_LineStrip
	// ==================
	arrIdx[0] = 0; 	arrIdx[1] = 1;	arrIdx[2] = 2;	arrIdx[3] = 3; arrIdx[4] = 0;
	pMesh = NEW AMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 5);
	AddAsset(L"RectMesh_LineStrip", pMesh.Get());

	// ==========
	// 삼각형 메쉬
	// ==========
	Vtx arr[3] = {};
	arr[0].vPos = Vec3(0.f, 1.f, 0.f);
	arr[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	arr[1].vPos = Vec3(1.f, -1.f, 0.f);
	arr[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	arr[2].vPos = Vec3(-1.f, -1.f, 0.f);
	arr[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	UINT idx[3] = { 0 , 1 , 2 };

	pMesh = NEW AMesh;
	pMesh->Create(arr, 3, idx, 3);
	AddAsset(L"TriMesh", pMesh.Get());

	// ===============
	// 원 (CircleMesh)
	// ===============
	vector<Vtx>	vecVtx;
	vector<UINT> vecIdx;

	// 중점
	Vtx v;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float Theta = 0.f;
	float Radius = 0.5f;
	float Slice = 40.f;

	// 원의 테두리 정점 추가
	for (int i = 0, end = static_cast<int>(Slice + 1); i < end; ++i) {
		v.vPos = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
		//v.vUV = Vec2(0.5f, 0.5f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		Theta += XM_2PI / Slice;
	}	

	// 인덱스
	for (int i = 0, end = static_cast<int>(Slice); i < end; ++i) {
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = NEW AMesh;
	pMesh->Create(
		vecVtx.data(), static_cast<UINT>(vecVtx.size()), 
		vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddAsset(L"CircleMesh", pMesh.Get());


	// ====================
	// CircleMesh_LineStrip
	// ====================
	vecIdx.clear();
	for (int i = 0, end = static_cast<int>(Slice + 1); i < end; ++i)
		vecIdx.push_back(i + 1);

	pMesh = NEW AMesh;
	pMesh->Create(
		vecVtx.data(), static_cast<UINT>(vecVtx.size()),
		vecIdx.data(), static_cast<UINT>(vecIdx.size()));
	AddAsset(L"CircleMesh_LineStrip", pMesh.Get());
}

void AssetMgr::CreateEngineShader() {	
	Ptr<AGraphicShader> pShader{};

	// ===========
	// Std2DShader
	// ===========
	pShader = NEW AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(ERasterizerState::E_CullNone);

	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");

	AddAsset(L"Std2DShader", pShader.Get());
	
	// ===============
	// BillboardShader
	// ===============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"BillboardShader");
	pShader->CreateVertexShader(L"Shader\\billboard.fx", "VS_Billboard");
	pShader->CreatePixelShader(L"Shader\\billboard.fx", "PS_Billboard");
	pShader->SetBSType(EBlendState::E_Default);
	pShader->SetRSType(ERasterizerState::E_CullNone);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// ============
	// SpriteShader
	// ============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"SpriteShader");
	pShader->CreateVertexShader(L"Shader\\sprite.fx", "VS_Sprite");
	pShader->CreatePixelShader(L"Shader\\sprite.fx", "PS_Sprite");
	pShader->SetBSType(EBlendState::E_Default);
	pShader->SetRSType(ERasterizerState::E_CullNone);

	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// ==============
	// FlipbookShader
	// ==============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"FlipbookShader");
	pShader->CreateVertexShader(L"Shader\\flipbook.fx","VS_Flipbook");
	pShader->CreatePixelShader(L"Shader\\flipbook.fx", "PS_Flipbook");
	pShader->SetBSType(EBlendState::E_Default);
	pShader->SetRSType(ERasterizerState::E_CullNone);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// =============
	// TileMapShader
	// =============
	// 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해둔다

	pShader = NEW AGraphicShader;
	pShader->SetName(L"TileShader");
	pShader->CreateVertexShader(L"Shader\\tile.fx", "VS_Tile");
	pShader->CreatePixelShader(L"Shader\\tile.fx",  "PS_Tile");
	pShader->SetBSType(EBlendState::E_Default);
	pShader->SetRSType(ERasterizerState::E_CullNone);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// ===============
	// DbgRenderShader
	// ===============
	pShader = NEW AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\dbg.fx", "VS_Debug");
	pShader->CreatePixelShader(L"Shader\\dbg.fx", "PS_Debug");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(ERasterizerState::E_CullNone);
	pShader->SetDSType(EDepthStencilState::E_NeverWrite);
	pShader->SetBSType(EBlendState::E_Default);

	AddAsset(L"DbgShader", pShader.Get());
}

void AssetMgr::CreateEngineTexture() {
	Load<ATexture>(L"PlayerImage", L"Texture\\Character.png");
	Load<ATexture>(L"Fighter", L"Texture\\Fighter.bmp");
	Load<ATexture>(L"Missile", L"Texture\\missile.png");
	Load<ATexture>(L"Link", L"Texture\\link.png");
	Load<ATexture>(L"TileAtlas", L"Texture\\TILE.bmp");
	Load<ATexture>(L"Ghost", L"Texture\\ghost.png");
}

void AssetMgr::CreateEngineMaterial() {
	Ptr<AMaterial> pMtrl{};

	// =========
	// Std2DMtrl
	// =========
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"Std2DMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));

	// Parameter
	pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\Fighter.bmp"));

	pMtrl->SetDomain(ERenderDomain::E_Masked);
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	// ===========
	// MonsterMtrl
	// ===========
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"MonsterMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));

	// Parameter
	pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\Character.png"));

	pMtrl->SetDomain(ERenderDomain::E_Masked);
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	// =======
	// DbgMtrl 
	// =======
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"DbgMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"DbgShader"));
	pMtrl->SetDomain(ERenderDomain::E_Debug);
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	///////////////////
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"EnemyMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\ghost.png"));

	pMtrl->SetDomain(ERenderDomain::E_Masked);
	AddAsset(pMtrl->GetName(), pMtrl.Get());
}

void AssetMgr::LoadTextures() {
	LoadAssets(L"Texture\\", 
		{ 
			".png",
			".jpg",
			".jpeg",
			".bmp" 
		}
		, [&](const wstring& path) {
			auto texture = LOAD(ATexture, path.c_str());
			AddAsset(texture->GetKey(), texture.Get());
		});
}

void AssetMgr::LoadSprites() {
	LoadAssets(L"Sprite\\", { ".sprite" }, [&](const wstring& path) {
		auto sprite = LOAD(ASprite, path.c_str());
		AddAsset(sprite->GetKey(), sprite.Get());
	});
}

void AssetMgr::LoadTileMaps() {
	LoadAssets(L"TileMap\\", { ".tile" }, [&](const wstring& path) {
		auto tile = LOAD(ATileMap, path.c_str());
		AddAsset(tile->GetKey(), tile.Get());
		});
}

void AssetMgr::LoadLevels() {
	LoadAssets(L"Level\\", { ".lv" }, [&](const wstring& path) {
		auto level = LOAD(ALevel, path.c_str());
		AddAsset(level->GetKey(), level.Get());
	});
}

void AssetMgr::LoadAssets(
	const wstring& folder
	, const unordered_set<string>& extensions
	, const std::function<void(const wstring&)>& func) {
	auto contentPath = wstring(CONTENT_PATH);
	auto folderPath = format(L"{}{}", contentPath, folder);
	auto root = filesystem::path(folderPath);
	if (!filesystem::exists(root)) return;

	for (const auto& entry : filesystem::recursive_directory_iterator(root)) {
		if (!entry.is_regular_file()) continue;

		std::string ext = entry.path().extension().string();
		if (extensions.contains(ext)) {
			auto filePath = StrToWStr(entry.path().string());
			auto filePathAfterConent = filePath.substr(contentPath.size());

			func(filePathAfterConent);
		}
	}
}