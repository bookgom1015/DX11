#include "pch.h"
#include "LevelMgr.h"

#include "GameObject.h"

#include "AssetMgr.h"
#include "Device.h"
#include "CollisionMgr.h"

#include "CTrackingCameraScript.h"
#include "CPlatformerPlayerScript.h"
#include "CEnemyScript.h"
#include "CFlashLightScript.h"
#include "CElevationScript.h"

LevelMgr::LevelMgr() {
	m_LevelState = ELevelState::E_Playing;
}

LevelMgr::~LevelMgr() {}

void LevelMgr::Init() {
	// Level 생성
	Ptr<ALevel> level = new ALevel;
	level->SetName(L"Current Level");

	level->GetLayer(0)->SetName(L"Default");
	level->GetLayer(1)->SetName(L"Background");
	level->GetLayer(2)->SetName(L"Tile");
	level->GetLayer(3)->SetName(L"Player");
	level->GetLayer(4)->SetName(L"PlayerProjectile");
	level->GetLayer(5)->SetName(L"Enermy");
	level->GetLayer(6)->SetName(L"EnermyProjectile");

	CreateGrounds(level);
	CreateLights(level);
	CreateEnemies(level);
	CreatePlayer(level);

	// Tile Object
	auto pTileObj = new GameObject;
	pTileObj->SetName(L"BackgroundTile");

	pTileObj->AddComponent(new CTransform);
	pTileObj->AddComponent(new CTileRender);

	pTileObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	auto pTileMap = new ATileMap;
	pTileMap->SetName(L"BackgroundTileMap");
	pTileMap->SetTileSize(Vec2(64.f, 64.f));
	pTileMap->SetAtlas(FIND(ATexture, L"TileAtlas"));
	pTileMap->SetRowCol(100, 100);

	for (int i = 0; i < 100; ++i)
		for (int j = 0; j < 100; ++j)
			pTileMap->SetSprite(i, j, LOAD(ASprite, L"Sprite\\TileSprite_1.sprite"));

	auto tileSize = pTileMap->GetTileSize();
	pTileObj->TileRender()->SetTileMap(pTileMap);

	pTileObj->Transform()->SetRelativePos(Vec3(
		tileSize.x * 100 * -0.5f,
		tileSize.x * 100 * 0.5f,
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

	// 레벨을 변경
	Util::ChangeLevel(L"Level01");
	ChangeLevelState(ELevelState::E_Playing);
}

void LevelMgr::Progress() {
	if (m_CurLevel == nullptr) return;

	// 이전에 등록된 모든 오브젝트들 제거
	m_CurLevel->Deregister();

	if (m_LevelState == ELevelState::E_Playing)
		m_CurLevel->Tick();

	m_CurLevel->FinalTick();

	if (m_LevelState == ELevelState::E_Playing)
		CollisionMgr::GetInst()->Progress(m_CurLevel);
}

Ptr<GameObject> LevelMgr::FindObjectByName(const wstring& _name) {
	return m_CurLevel->FindObjectByName(_name);
}

void LevelMgr::ChangeLevel(Ptr<ALevel> level) {
	m_CurLevel = m_SharedLevel = level;
	m_LevelState = ELevelState::E_Stopped;
}

void LevelMgr::ChangeLevelState(ELevelState::Type state) {
	if (m_LevelState == state) return;

	// Stop -> Play
	if (m_LevelState == ELevelState::E_Stopped && state == ELevelState::E_Playing) {
		// 원본 에셋 레벨의 복제본 레벨을 만들어서 현재 레벨로 가리킨다.
		//m_CurLevel = m_SharedLevel->Clone();
		m_CurLevel->Begin();
	}
	else if ((m_LevelState == ELevelState::E_Playing || m_LevelState == ELevelState::E_Paused)
		&& state == ELevelState::E_Stopped) {
		m_CurLevel = m_SharedLevel;
	}

	m_LevelState = state;
}

void LevelMgr::CreatePlayer(Ptr<ALevel> level) {
	// 플레이어 생성
	auto player = new GameObject;
	player->SetName(L"Player");
	player->AddComponent(new CTransform);
	player->AddComponent(new CFlipbookRender);
	player->AddComponent(new CCollider2D);
	player->AddComponent(new CRigidBody);
	player->AddComponent(new CPlatfomerPlayerScript);
	player->AddComponent(new CFlashLightScript);

	player->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	player->Transform()->SetRelativePos(Vec3(0.f, -105.f, 0.f));

	player->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	player->Collider2D()->SetScale(Vec2(0.65f, 0.78f));

	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveDown.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveLeft.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveUp.flip"));
	player->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveRight.flip"));

	// 플레이어 손 생성
	Ptr<GameObject> arm = new GameObject;
	arm->SetName(L"Arm");
	arm->AddComponent(new CTransform);

	arm->Transform()->SetIndependentScale(true);

	Ptr<GameObject> hand = new GameObject;
	hand->SetName(L"Hand");
	hand->AddComponent(new CTransform);
	hand->AddComponent(new CMeshRender);
	hand->AddComponent(new CCollider2D);

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
		auto camera = new GameObject;
		camera->SetName(L"MainCamera");

		camera->AddComponent(new CTransform);
		camera->AddComponent(new CCamera);
		auto tracking = new CTrackingCameraScript;
		camera->AddComponent(tracking);

		camera->Camera()->LayerCheckAll();

		camera->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		camera->Camera()->SetFar(10000.f);
		camera->Camera()->SetFOV(90.f);
		camera->Camera()->SetOrthoScale(1.f);
		Vec2 vResolution = Device::GetInst()->GetRenderResolution();
		camera->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
		camera->Camera()->SetWidth(vResolution.x);

		m_MainCam = camera;
		level->AddObject(ELevelLayer::E_Default, camera);

		// 카메라 타겟 설정
		tracking->SetTarget(player);
	}
}

void LevelMgr::CreateGrounds(Ptr<ALevel> level) {
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(0.f, -400.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(3000.f, 500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(3000.f, 500.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground1");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(600.f, 200.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(500.f, 100.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground2");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		auto elevation = new CElevationScript;
		elevation->SetStopOverPos(Vec3(210.f, 360.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(210.f, 200.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(100.f, 100.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground3");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		auto elevation = new CElevationScript;
		elevation->SetStopOverPos(Vec3(-120.f, 450.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(-120.f, 250.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(100.f, 100.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground4");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(-500.f, 400.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 100.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(500.f, 100.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground5");

		// 레벨 시작
		level->SetChanged();
		level->Begin();

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		auto elevation = new CElevationScript;
		elevation->SetStopOverPos(Vec3(250.f, 650.f, 0.f));
		elevation->SetSpeed(0.5f);

		pGround->AddComponent(elevation);

		pGround->Transform()->SetRelativePos(Vec3(0.f, 650.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(50.f, 50.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"LeftWall");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3(-(1000.f + 250.f), 450.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 1500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(500.f, 1500.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"RightWall");

		pGround->AddComponent(new CTransform);
		pGround->AddComponent(new CBillboardRender);
		pGround->AddComponent(new CCollider2D);

		pGround->Transform()->SetRelativePos(Vec3((1000.f + 250.f), 450.f, 0.f));
		pGround->Transform()->SetRelativeScale(Vec3(500.f, 1500.f, 0.f));
		pGround->BillboardRender()->SetBillboardScale(Vec2(500.f, 1500.f));
		pGround->BillboardRender()->SetAlbedo(Vec3(0.075f));

		level->AddObject(ELevelLayer::E_Ground, pGround);
	}
}

void LevelMgr::CreateLights(Ptr<ALevel> level) {
	{
		auto pObject = new GameObject;
		pObject->SetName(L"DirectionalLight1");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
		pObject->Light2D()->SetLightColor(Vec3(0.f));
		pObject->Light2D()->SetAmbient(Vec3(1.f) * 0.025f);

		level->AddObject(ELevelLayer::E_Light, pObject);
	}
	{
		auto pObject = new GameObject;
		pObject->SetName(L"LineLight1");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);
		pObject->AddComponent(new CCollider2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::LINE);
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
		auto pObject = new GameObject;
		pObject->SetName(L"RectLight1");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);
		pObject->AddComponent(new CCollider2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::RECT);
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
		auto pObject = new GameObject;
		pObject->SetName(L"LineLight2");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);
		pObject->AddComponent(new CCollider2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::LINE);
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
		auto pObject = new GameObject;
		pObject->SetName(L"RectLight2");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);
		pObject->AddComponent(new CCollider2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::RECT);
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
	//	auto pObject = new GameObject;
	//	pObject->SetName(L"PointLight1");
	//	pObject->AddComponent(new CTransform);
	//	pObject->AddComponent(new CLight2D);
	//	pObject->AddComponent(new CCollider2D);
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
	{
		auto pEnemy = new GameObject;
		pEnemy->SetName(L"Enemy1");
		pEnemy->AddComponent(new CTransform);
		pEnemy->AddComponent(new CMeshRender);
		pEnemy->AddComponent(new CCollider2D);
		pEnemy->AddComponent(new CRigidBody);
		pEnemy->AddComponent(new CEnemyScript);

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
		auto pEnemy = new GameObject;
		pEnemy->SetName(L"Enemy2");
		pEnemy->AddComponent(new CTransform);
		pEnemy->AddComponent(new CMeshRender);
		pEnemy->AddComponent(new CCollider2D);
		pEnemy->AddComponent(new CRigidBody);
		pEnemy->AddComponent(new CEnemyScript);

		pEnemy->Transform()->SetRelativePos(Vec3(500.f, 325.f, 0.f));
		pEnemy->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

		pEnemy->Collider2D()->SetScale(Vec2(0.65f, 0.75f));

		pEnemy->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
		pEnemy->MeshRender()->SetMaterial(FIND(AMaterial, L"EnemyMtrl"));
		pEnemy->MeshRender()->SetAlbedo(Vec4(-0.5f, -0.5f, -0.5f, 0.95f));

		level->AddObject(ELevelLayer::E_Enemy, pEnemy);
	}
}