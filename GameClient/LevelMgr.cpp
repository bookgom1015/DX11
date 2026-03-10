#include "pch.h"
#include "LevelMgr.h"

#include "AssetMgr.h"
#include "EditorMgr.h"
#include "PathMgr.h"
#include "CollisionMgr.h"

#include "Device.h"

#include "GameObject.h"

#include "Inspector.h"

#include "Source/Scripts/CTrackingCameraScript.h"
#include "Source/Scripts/CPlatformerPlayerScript.h"
#include "Source/Scripts/CEnemyScript.h"
#include "Source/Scripts/CFlashLightScript.h"
#include "Source/Scripts/CElevationScript.h"

LevelMgr::LevelMgr() 
	: m_CurLevel{ nullptr }
	, m_SharedLevel{ nullptr }
	, m_LevelState{ ELevelState::E_Playing }
	, m_LevelResetRequested{ true } {}

LevelMgr::~LevelMgr() {}

void LevelMgr::Init() {
	if (true) {
		Ptr<ALevel> pLevel = LOAD(ALevel, L"Level\\Level01.lv");	
		Util::ChangeLevel(L"Level\\Level01.lv");

		return;
	}

	// Level 생성
	Ptr<ALevel> level = NEW ALevel;
	level->SetName(L"Current Level");

	level->GetLayer(ELevelLayer::E_Default)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Default));
	level->GetLayer(ELevelLayer::E_Light)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Light));
	level->GetLayer(ELevelLayer::E_Player)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Player));
	level->GetLayer(ELevelLayer::E_Enemy)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Enemy));
	level->GetLayer(ELevelLayer::E_Ground)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Ground));
	level->GetLayer(ELevelLayer::E_Projectile)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Projectile));
	level->GetLayer(ELevelLayer::E_Particle)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Particle));
	level->GetLayer(ELevelLayer::E_Background)->SetName(
		ELevelLayer::GetLevelLayerName(ELevelLayer::E_Background));

	CreateGrounds(level);
	CreateLights(level);
	CreateEnemies(level);
	CreatePlayer(level);

	// Tile Object
	auto pTileObj = NEW GameObject;
	pTileObj->SetName(L"BackgroundTile");

	pTileObj->AddComponent(NEW CTransform);
	pTileObj->AddComponent(NEW CTileRender);

	pTileObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	auto pTileMap = LOAD(ATileMap, L"TileMap\\TestTileMap.tile");
	//pTileMap->SetName(L"BackgroundTileMap");
	//pTileMap->SetTileSize(Vec2(64.f, 64.f));
	//pTileMap->SetAtlas(FIND(ATexture, L"TileAtlas"));
	//pTileMap->SetRowCol(100, 100);
	//
	//for (int i = 0; i < 100; ++i)
	//	for (int j = 0; j < 100; ++j)
	//		pTileMap->SetSprite(i, j, LOAD(ASprite, L"Sprite\\TileSprite_1.sprite"));
	//
	//{
	//	wstring contentPath = CONTENT_PATH;
	//	pTileMap->Save(contentPath + L"TileMap\\TestTileMap.tile");
	//}

	auto tileSize = pTileMap->GetTileSize();
	pTileObj->TileRender()->SetTileMap(pTileMap);

	pTileObj->Transform()->SetRelativePos(Vec3(
		tileSize.x * 100 * -0.5f,
		tileSize.y * 100 * 0.5f,
		100.f));

	level->AddObject(ELevelLayer::E_Background, pTileObj);

	// 충돌 설정
	level->CheckCollisionLayer(ELevelLayer::E_Player, ELevelLayer::E_Ground);
	level->CheckCollisionLayer(ELevelLayer::E_Enemy, ELevelLayer::E_Ground);
	level->CheckCollisionLayer(ELevelLayer::E_Enemy, ELevelLayer::E_Projectile);
	level->CheckCollisionLayer(ELevelLayer::E_Ground, ELevelLayer::E_Projectile);
	level->CheckCollisionLayer(ELevelLayer::E_Ground, ELevelLayer::E_Particle);

	// 레벨 변경점 체크
	level->SetChanged();

	AssetMgr::GetInst()->AddAsset(L"Level01", level.Get());

	wstring contentPath = CONTENT_PATH;
	level->Save(contentPath + L"Level\\Level01.lv");

	// 레벨을 변경
	Util::ChangeLevel(L"Level01");
}

void LevelMgr::Progress() {
	if (m_CurLevel == nullptr) return;

	// 이전에 등록된 모든 오브젝트들 제거
	m_CurLevel->Deregister();

	if (m_LevelState == ELevelState::E_Playing) m_CurLevel->Tick();

	m_CurLevel->FinalTick();

	if (m_LevelState == ELevelState::E_Playing)	m_CurLevel->LateTick();

	if (m_LevelState == ELevelState::E_Playing)
		CollisionMgr::GetInst()->Progress(m_CurLevel);
}

Ptr<GameObject> LevelMgr::FindObjectByName(const wstring& _name) {
	return m_CurLevel->FindObjectByName(_name);
}

void LevelMgr::ChangeLevel(Ptr<ALevel> level) {
	m_CurLevel = m_SharedLevel = level;
	m_LevelState = ELevelState::E_Stopped;
	level->SetChanged();
}

void LevelMgr::ChangeLevelState(ELevelState::Type state) {
	if (m_LevelState == state) return;

	// Stop -> Play
	if (state == ELevelState::E_Playing && m_LevelResetRequested) {
		m_LevelResetRequested = false;

		// 원본 에셋 레벨의 복제본 레벨을 만들어서 현재 레벨로 가리킨다.
		m_CurLevel = m_SharedLevel->Clone();
		m_CurLevel->SetChanged();
		m_CurLevel->Begin();

		auto ui = EditorMgr::GetInst()->FindUI("Inspector");
		auto inspector = static_cast<Inspector*>(ui.Get());
		inspector->NeedToResetTarget();
	}
	else if (state == ELevelState::E_Stopped) {
		m_LevelResetRequested = true;

		m_CurLevel = m_SharedLevel;
		m_CurLevel->SetChanged();

		auto ui = EditorMgr::GetInst()->FindUI("Inspector");
		auto inspector = static_cast<Inspector*>(ui.Get());
		inspector->NeedToResetTarget();
	}

	m_LevelState = state;
}

void LevelMgr::CreatePlayer(Ptr<ALevel> level) {
	// 플레이어 생성
	auto player = NEW GameObject;
	player->SetName(L"Player");
	player->AddComponent(NEW CTransform);
	player->AddComponent(NEW CFlipbookRender);
	player->AddComponent(NEW CCollider2D);
	player->AddComponent(NEW CRigidBody);
	player->AddComponent(NEW CPlatformerPlayerScript);
	player->AddComponent(NEW CFlashLightScript);

	player->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	player->Transform()->SetRelativePos(Vec3(0.f, -105.f, 0.f));

	player->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	player->Collider2D()->SetScale(Vec2(0.65f, 0.78f));

	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveDown.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveLeft.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveUp.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveRight.flip"));

	// 플레이어 손 생성
	Ptr<GameObject> arm = NEW GameObject;
	arm->SetName(L"Arm");
	arm->AddComponent(NEW CTransform);

	arm->Transform()->SetIndependentScale(true);

	Ptr<GameObject> hand = NEW GameObject;
	hand->SetName(L"Hand");
	hand->AddComponent(NEW CTransform);
	hand->AddComponent(NEW CMeshRender);
	hand->AddComponent(NEW CCollider2D);

	hand->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	hand->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -XM_PIDIV2));
	hand->Transform()->SetRelativePos(Vec3(60.f, 0.f, 0.f));
	hand->Transform()->SetIndependentScale(true);

	hand->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	hand->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	arm->AddChild(hand);
	player->AddChild(arm);

	// 플레이어 레벨 추가
	level->AddObject(ELevelLayer::E_Player, player);

	{
		// 카메라 역할 오브젝트 
		auto camera = NEW GameObject;
		camera->SetName(L"MainCamera");

		camera->AddComponent(NEW CTransform);
		camera->AddComponent(NEW CCamera);
		auto tracking = NEW CTrackingCameraScript;
		camera->AddComponent(tracking);

		camera->Camera()->LayerCheckAll();

		camera->Camera()->SetProjType(EProjection::E_Orthographic);
		camera->Camera()->SetFar(10000.f);
		camera->Camera()->SetFOV(90.f);
		camera->Camera()->SetOrthoScale(1.f);
		Vec2 vResolution = Device::GetInst()->GetRenderResolution();
		camera->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
		camera->Camera()->SetWidth(vResolution.x);

		level->AddObject(ELevelLayer::E_Default, camera);
	}
}

void LevelMgr::CreateGrounds(Ptr<ALevel> level) {
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(0.f, -400.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(3000.f, 500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground1");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(600.f, 200.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground2");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		auto elevation = NEW CElevationScript;
		elevation->SetStopOverPos(Vec3(210.f, 360.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(210.f, 200.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground3");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		auto elevation = NEW CElevationScript;
		elevation->SetStopOverPos(Vec3(-120.f, 450.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(-120.f, 250.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground4");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(-500.f, 400.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"Ground5");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		auto elevation = NEW CElevationScript;
		elevation->SetStopOverPos(Vec3(250.f, 650.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(0.f, 650.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"LeftWall");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(-(1000.f + 250.f), 450.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 1500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = NEW GameObject;
		pGround->SetName(L"RightWall");

		pGround->AddComponent(NEW CTransform);
		pGround->AddComponent(NEW CBillboardRender);
		pGround->AddComponent(NEW CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3((1000.f + 250.f), 450.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 1500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(1.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
}

void LevelMgr::CreateLights(Ptr<ALevel> level) {
	{
		auto pObject = NEW GameObject;
		pObject->SetName(L"DirectionalLight1");
		pObject->AddComponent(NEW CTransform);
		pObject->AddComponent(NEW CLight2D);

		pObject->Light2D()->SetLightType(ELight::E_Directional);
		pObject->Light2D()->SetLightColor(Vec3(0.f));
		pObject->Light2D()->SetAmbient(Vec3(1.f) * 0.025f);

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	{
		auto pObject = NEW GameObject;
		pObject->SetName(L"LineLight1");
		pObject->AddComponent(NEW CTransform);
		pObject->AddComponent(NEW CLight2D);
		pObject->AddComponent(NEW CCollider2D);

		pObject->Light2D()->SetLightType(ELight::E_Line);
		pObject->Light2D()->SetLightColor(Vec3(1.f));
		pObject->Light2D()->SetIntensity(0.19f);
		pObject->Light2D()->SetLength(600.f);
		pObject->Light2D()->SetLightDir(Vec3(0.f, -1.f, 0.f));
		pObject->Light2D()->SetRadius(1000.f);

		pObject->Transform()->SetRelativePos(Vec3(600.f, 145.f, 0.f));
		pObject->Transform()->SetRelativeScale(Vec3(500.f, 10.f, 0.f));

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	{
		auto pObject = NEW GameObject;
		pObject->SetName(L"RectLight1");
		pObject->AddComponent(NEW CTransform);
		pObject->AddComponent(NEW CLight2D);
		pObject->AddComponent(NEW CCollider2D);

		pObject->Light2D()->SetLightType(ELight::E_Rect);
		pObject->Light2D()->SetLightColor(Vec3(1.f));
		pObject->Light2D()->SetIntensity(0.75f);
		pObject->Light2D()->SetLightDir(Vec3(1.f, 0.f, 0.f));
		pObject->Light2D()->SetLength(100.f);
		pObject->Light2D()->SetRadius(25.f);

		pObject->Transform()->SetRelativePos(Vec3(700.f, 300.f, 0.f));
		pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	{
		auto pObject = NEW GameObject;
		pObject->SetName(L"LineLight2");
		pObject->AddComponent(NEW CTransform);
		pObject->AddComponent(NEW CLight2D);
		pObject->AddComponent(NEW CCollider2D);

		pObject->Light2D()->SetLightType(ELight::E_Line);
		pObject->Light2D()->SetLightColor(Vec3(1.f));
		pObject->Light2D()->SetIntensity(0.19f);
		pObject->Light2D()->SetLength(500.f);
		pObject->Light2D()->SetLightDir(Vec3(0.f, -1.f, 0.f));
		pObject->Light2D()->SetRadius(1000.f);

		pObject->Transform()->SetRelativePos(Vec3(-500.f, 345.f, 0.f));
		pObject->Transform()->SetRelativeScale(Vec3(500.f, 10.f, 0.f));

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	{
		auto pObject = NEW GameObject;
		pObject->SetName(L"RectLight2");
		pObject->AddComponent(NEW CTransform);
		pObject->AddComponent(NEW CLight2D);
		pObject->AddComponent(NEW CCollider2D);

		pObject->Light2D()->SetLightType(ELight::E_Rect);
		pObject->Light2D()->SetLightColor(Vec3(1.f));
		pObject->Light2D()->SetIntensity(0.75f);
		pObject->Light2D()->SetLightDir(Vec3(1.f, 0.f, 0.f));
		pObject->Light2D()->SetLength(100.f);
		pObject->Light2D()->SetRadius(25.f);

		pObject->Transform()->SetRelativePos(Vec3(-600.f, 500.f, 0.f));
		pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	//{
	//	auto pObject = NEW GameObject;
	//	pObject->SetName(L"PointLight1");
	//	pObject->AddComponent(NEW CTransform);
	//	pObject->AddComponent(NEW CLight2D);
	//	pObject->AddComponent(NEW CCollider2D);
	//
	//	pObject->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	//	pObject->Light2D()->SetLightColor(Vec3(1.f));
	//	pObject->Light2D()->SetIntensity(0.45f);
	//	pObject->Light2D()->SetRadius(1000.f);
	//
	//	pObject->Transform()->SetRelativePos(Vec3(35.f, 400.f, 0.f));
	//	pObject->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	//
	//	level->AddObject(ELevelLayer::E_Light, pObject);
	//}
}

void LevelMgr::CreateEnemies(Ptr<ALevel> level) {
	//{
	//	auto pEnemy = NEW GameObject;
	//	pEnemy->AddComponent(NEW CTransform);
	//	pEnemy->AddComponent(NEW CMeshRender);
	//	pEnemy->AddComponent(NEW CCollider2D);
	//	pEnemy->AddComponent(NEW CRigidBody);
	//	pEnemy->AddComponent(NEW CEnemyScript);
	//
	//	pEnemy->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	//
	//	pEnemy->Collider2D()->SetScale(Vec2(0.65f, 0.75f));
	//
	//	pEnemy->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	//	pEnemy->MeshRender()->SetMaterial(FIND(AMaterial, L"EnemyMtrl"));
	//	pEnemy->MeshRender()->SetAlbedo(Vec4(-0.5f, -0.5f, -0.5f, 0.95f));
	//
	//	AssetMgr::GetInst()->AddAsset(L"Enemy");
	//}
	{
		auto pEnemy = NEW GameObject;
		pEnemy->SetName(L"Enemy1");
		pEnemy->AddComponent(NEW CTransform);
		pEnemy->AddComponent(NEW CMeshRender);
		pEnemy->AddComponent(NEW CCollider2D);
		pEnemy->AddComponent(NEW CRigidBody);
		pEnemy->AddComponent(NEW CEnemyScript);

		pEnemy->Transform()->SetRelativePos(Vec3(-400.f, 525.f, 0.f));
		pEnemy->Transform()->SetRelativeRot(Vec3(0.f, XM_PI, 0.f));
		pEnemy->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

		pEnemy->Collider2D()->SetScale(Vec2(0.65f, 0.75f));

		pEnemy->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
		pEnemy->MeshRender()->SetMaterial(FIND(AMaterial, L"EnemyMtrl"));
		pEnemy->MeshRender()->SetAlbedo(Vec4(-0.5f, -0.5f, -0.5f, 0.95f));

		level->AddObject(ELevelLayer::E_Enemy, pEnemy);
	}
	{
		auto pEnemy = NEW GameObject;
		pEnemy->SetName(L"Enemy2");
		pEnemy->AddComponent(NEW CTransform);
		pEnemy->AddComponent(NEW CMeshRender);
		pEnemy->AddComponent(NEW CCollider2D);
		pEnemy->AddComponent(NEW CRigidBody);
		pEnemy->AddComponent(NEW CEnemyScript);

		pEnemy->Transform()->SetRelativePos(Vec3(500.f, 325.f, 0.f));
		pEnemy->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

		pEnemy->Collider2D()->SetScale(Vec2(0.65f, 0.75f));

		pEnemy->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
		pEnemy->MeshRender()->SetMaterial(FIND(AMaterial, L"EnemyMtrl"));
		pEnemy->MeshRender()->SetAlbedo(Vec4(-0.5f, -0.5f, -0.5f, 0.95f));

		level->AddObject(ELevelLayer::E_Enemy, pEnemy);
	}
}