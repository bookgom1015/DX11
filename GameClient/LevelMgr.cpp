#include "pch.h"
#include "LevelMgr.h"

#include "GameObject.h"

#include "AssetMgr.h"
#include "Device.h"
#include "CollisionMgr.h"

#include "CPlayerScript.h"
#include "CCamMoveScript.h"
#include "CMonsterScript.h"

#include "CTrackingCameraScript.h"
#include "CPlatformerPlayerScript.h"
#include "CEnemyScript.h"
#include "CFlashLightScript.h"
#include "CElevationScript.h"

LevelMgr::LevelMgr()
{
}

LevelMgr::~LevelMgr()
{
}

void LevelMgr::Init()
{
	// Level 생성
	m_CurLevel = new ALevel;
	m_CurLevel->SetName(L"Current Level");

	m_CurLevel->GetLayer(0)->SetName(L"Default");
	m_CurLevel->GetLayer(1)->SetName(L"Background");
	m_CurLevel->GetLayer(2)->SetName(L"Tile");
	m_CurLevel->GetLayer(3)->SetName(L"Player");
	m_CurLevel->GetLayer(4)->SetName(L"PlayerProjectile");
	m_CurLevel->GetLayer(5)->SetName(L"Enermy");
	m_CurLevel->GetLayer(6)->SetName(L"EnermyProjectile");

	Ptr<GameObject> pObject = nullptr;	
	
	// 카메라 역할 오브젝트 
	pObject = new GameObject;
	pObject->SetName(L"MainCamera");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCamera);
	auto tracking = new CTrackingCameraScript;
	pObject->AddComponent(tracking);

	pObject->Camera()->LayerCheckAll();
	//pObject->Camera()->LayerCheck(0); 
	//pObject->Camera()->LayerCheck(1); 
	//pObject->Camera()->LayerCheck(2);

	pObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pObject->Camera()->SetFar(10000.f);
	pObject->Camera()->SetFOV(90.f);
	pObject->Camera()->SetOrthoScale(1.f);
	Vec2 vResolution = Device::GetInst()->GetRenderResolution();
	pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
	pObject->Camera()->SetWidth(vResolution.x);
	
	m_MainCam = pObject;
	m_CurLevel->AddObject(ELevelLayer::E_Default, pObject);

	CreateGrounds();
	CreateLights();
	CreateEnemies();

	// 플레이어 생성
	Ptr<GameObject> pPlayer = new GameObject;
	pPlayer->SetName(L"Player");
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CFlipbookRender);
	pPlayer->AddComponent(new CCollider2D);
	pPlayer->AddComponent(new CRigidBody);
	pPlayer->AddComponent(new CPlatfomerPlayerScript);
	pPlayer->AddComponent(new CFlashLightScript);

	pPlayer->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	pPlayer->Transform()->SetRelativePos(Vec3(0.f, -105.f, 0.f));

	pPlayer->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	pPlayer->Collider2D()->SetScale(Vec2(0.65f, 0.78f));

	pPlayer->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveDown.flip"));
	pPlayer->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveLeft.flip"));
	pPlayer->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveUp.flip"));
	pPlayer->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveRight.flip"));

	// 플레이어 손 생성
	Ptr<GameObject> pArm = new GameObject;
	pArm->SetName(L"Arm");
	pArm->AddComponent(new CTransform);

	pArm->Transform()->SetIndependentScale(true);

	Ptr<GameObject> pHand = new GameObject;
	pHand->SetName(L"Hand");
	pHand->AddComponent(new CTransform);
	pHand->AddComponent(new CMeshRender);
	pHand->AddComponent(new CCollider2D);

	pHand->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	pHand->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -XM_PIDIV2));
	pHand->Transform()->SetRelativePos(Vec3(60.f, 0.f, 0.f));
	pHand->Transform()->SetIndependentScale(true);

	pHand->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	pHand->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	pArm->AddChild(pHand);
	pPlayer->AddChild(pArm);

	// 플레이어 레벨 추가
	m_CurLevel->AddObject(ELevelLayer::E_Player, pPlayer);

	// 카메라 타겟 설정
	tracking->SetTarget(pPlayer);

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

	m_CurLevel->AddObject(ELevelLayer::E_Background, pTileObj);

	// 충돌 설정
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Player, ELevelLayer::E_Ground);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Enemy, ELevelLayer::E_Ground);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Enemy, ELevelLayer::E_Projectile);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Ground, ELevelLayer::E_Projectile);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Ground, ELevelLayer::E_Particle);

	// 레벨 시작
	m_CurLevel->SetChanged();
	m_CurLevel->Begin();
}

void LevelMgr::CreateGrounds() {
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
	}
	{
		auto pGround = new GameObject;
		pGround->SetName(L"Ground5");

		// 레벨 시작
		m_CurLevel->SetChanged();
		m_CurLevel->Begin();

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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
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

		m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);
	}
}

void LevelMgr::CreateLights() {
	{
		auto pObject = new GameObject;
		pObject->SetName(L"DirectionalLight1");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CLight2D);

		pObject->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
		pObject->Light2D()->SetLightColor(Vec3(0.f));
		pObject->Light2D()->SetAmbient(Vec3(1.f) * 0.025f);

		m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
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

		m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
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

		m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
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

		m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
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

		m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
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
	//	m_CurLevel->AddObject(ELevelLayer::E_Light, pObject);
	//}
}

void LevelMgr::CreateEnemies() {
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

		m_CurLevel->AddObject(ELevelLayer::E_Enemy, pEnemy);
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

		m_CurLevel->AddObject(ELevelLayer::E_Enemy, pEnemy);
	}
}

void LevelMgr::Progress()
{
	// 이전에 등록된 모든 오브젝트들 제거
	m_CurLevel->Deregister();

	// 레벨안에 있는 오브젝트들이 이번 DT 동안 할 일 수행
	m_CurLevel->Tick();
	m_CurLevel->FinalTick();

	// 충돌 검사 진행
	CollisionMgr::GetInst()->Progress(m_CurLevel);
}

Ptr<GameObject> LevelMgr::FindObjectByName(const wstring& _name)
{
	return m_CurLevel->FindObjectByName(_name);
}