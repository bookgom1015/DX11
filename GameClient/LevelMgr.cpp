#include "pch.h"
#include "LevelMgr.h"

#include "GameObject.h"

#include "AssetMgr.h"
#include "Device.h"
#include "CollisionMgr.h"

#include "CPlayerScript.h"
#include "CTrackingCameraScript.h"
#include "CPlatformerPlayerScript.h"

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
	pObject->SetName(L"Main Camera");

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
	mMainCam = pObject;

	m_CurLevel->AddObject(0, pObject);

	// 바닥 생성
	Ptr<GameObject> pGround = new GameObject;
	pGround->SetName(L"Ground");
	
	pGround->AddComponent(new CTransform);
	pGround->AddComponent(new CBillboardRender);
	pGround->AddComponent(new CCollider2D);
	
	pGround->Transform()->SetRelativePos(Vec3(0.f, -400.f, 0.f));
	pGround->Transform()->SetRelativeScale(Vec3(2000.f, 50.f, 0.f));
	pGround->BillboardRender()->SetBillboardScale(Vec2(2000.f, 50.f));
	
	m_CurLevel->AddObject(ELevelLayer::E_Ground, pGround);

	// 벽 생성
	Ptr<GameObject> leftWall = new GameObject;
	leftWall->SetName(L"LeftWall");

	leftWall->AddComponent(new CTransform);
	leftWall->AddComponent(new CBillboardRender);
	leftWall->AddComponent(new CCollider2D);

	leftWall->Transform()->SetRelativePos(Vec3(-(1000.f + 25.f), 75.f, 0.f));
	leftWall->Transform()->SetRelativeScale(Vec3(50.f, 1000.f, 0.f));
	leftWall->BillboardRender()->SetBillboardScale(Vec2(50.f, 1000.f));

	m_CurLevel->AddObject(ELevelLayer::E_Obstacle, leftWall);

	Ptr<GameObject> rightWall = new GameObject;
	rightWall->SetName(L"RightWall");

	rightWall->AddComponent(new CTransform);
	rightWall->AddComponent(new CBillboardRender);
	rightWall->AddComponent(new CCollider2D);

	rightWall->Transform()->SetRelativePos(Vec3((1000.f + 25.f), 75.f, 0.f));
	rightWall->Transform()->SetRelativeScale(Vec3(50.f, 1000.f, 0.f));
	rightWall->BillboardRender()->SetBillboardScale(Vec2(50.f, 1000.f));

	m_CurLevel->AddObject(ELevelLayer::E_Obstacle, rightWall);

	// 플레이어 생성
	Ptr<GameObject> pPlayer = new GameObject;
	pPlayer->SetName(L"Player");
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CMeshRender);
	pPlayer->AddComponent(new CCollider2D);	
	pPlayer->AddComponent(new CRigidBody);
	pPlayer->AddComponent(new CPlatfomerPlayerScript);
	
	pPlayer->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	
	pPlayer->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	pPlayer->Collider2D()->SetScale(Vec2(0.25f, 0.5f));
	
	pPlayer->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

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

	pHand->Transform()->SetRelativePos(Vec3(60.f, 0.f, 0.f));
	pHand->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	pHand->Transform()->SetIndependentScale(true);

	pHand->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	pHand->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	pArm->AddChild(pHand);
	pPlayer->AddChild(pArm);

	// 플레이어 레벨 추가
	m_CurLevel->AddObject(ELevelLayer::E_Player, pPlayer);

	// 카메라 타겟 설정
	tracking->SetTarget(pPlayer);

	// 충돌 설정
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Player, ELevelLayer::E_Ground);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Player, ELevelLayer::E_Obstacle);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Ground, ELevelLayer::E_Projectile);
	m_CurLevel->CheckCollisionLayer(ELevelLayer::E_Obstacle, ELevelLayer::E_Projectile);

	//// 몬스터 생성
	//Ptr<GameObject> pMonster = new GameObject;
	//pMonster->SetName(L"Monster");
	//
	//pMonster->AddComponent(new CTransform);
	//pMonster->AddComponent(new CBillboardRender);
	//pMonster->AddComponent(new CCollider2D);
	//
	//pMonster->Transform()->SetRelativePos(Vec3(300.f, 0.f, 100.f));
	//pMonster->Transform()->SetRelativeScale(Vec3(200.f, 50.f, 0.f));
	//pMonster->BillboardRender()->SetBillboardScale(Vec2(200.f, 50.f));
	//
	//m_CurLevel->AddObject(5, pMonster);
	//
	//// Player Object 추가
	//pObject = new GameObject;
	//pObject->SetName(L"Player");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CCollider2D);
	//
	//Ptr<CPlayerScript> pPlayerScript = new CPlayerScript;
	//pPlayerScript->SetTarget(pMonster);
	//pObject->AddComponent(pPlayerScript.Get());
	//
	//pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
	//pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	//
	//pObject->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	//pObject->Collider2D()->SetScale(Vec2(0.25f, 0.5f));
	//
	//pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
	//
	//
	//Ptr<GameObject> pChild = new GameObject;
	//pChild->SetName(L"Child");
	//pChild->AddComponent(new CTransform);
	//pChild->AddComponent(new CMeshRender);	
	//pChild->AddComponent(new CCollider2D);
	//
	//pChild->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
	//pChild->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	//pChild->Transform()->SetIndependentScale(true);
	//
	//pChild->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
	//pChild->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
	//
	//// Player 와 Child 부모자식 연결
	//pObject->AddChild(pChild);
	//
	//// Player(부모 오브젝트) 를 레벨에 추가
	//m_CurLevel->AddObject(3, pObject);
	//
	//
	//// 레벨 충돌 설정
	//m_CurLevel->CheckCollisionLayer(3, 5);
	//m_CurLevel->CheckCollisionLayer(4, 5);
	//m_CurLevel->CheckCollisionLayer(3, 6);

	// 레벨 시작
	m_CurLevel->Begin();
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
