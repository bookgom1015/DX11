#include "pch.h"
#include "CCamera.h"

#include "LevelMgr.h"
#include "ALevel.h"
#include "Layer.h"

#include "RenderMgr.h"

#include "CTransform.h"

#include "Device.h"


CCamera::CCamera()
	: Component(COMPONENT_TYPE::CAMERA)
	, m_LayerCheck(0)
{

}

CCamera::~CCamera()
{
}

void CCamera::Begin()
{
	// 레벨이 시작될때 호출됨
	// RenderMgr 에 카메라(본인)를 등록
	RenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::FinalTick()
{
	// 뷰 행렬 계산
	// 카메라의 위치
	Vec3 vPos = Transform()->GetRelativePos();

	// 이동 ( 카메라 위치를 원점으로 되돌리는 만큼의 이동행렬)
	Matrix matTrans = XMMatrixIdentity();
	matTrans._41 = -vPos.x; 
	matTrans._42 = -vPos.y; 
	matTrans._43 = -vPos.z;

	// View 행렬 회전
	//  - 카메라가 바라보는 방향을 z 축이 되도록 회전하는 부분이 추가
	// 카메라의 Right, Up, Front 방향 벡터에 회전행렬을 곱하면 이게 다시 x축, y축, z 축이 되는 회전행을 구해야 함
	 
	// vR				  ( 1 0 0)
	// vU    x    R   =   ( 0 1 0)
	// vZ				  ( 0 0 1)
								 
	//			vR
	// R 행렬은  vU     의 역핼렬
	//			vZ	
	
	//	vR
	//  vU   행렬은 행 끼리의 관계가 직교상태이기 때문에, 전치(Transpose) 를 통해서 역행렬을 쉽게 구할 수 있음
	//	vZ

	// 전치한 행렬과 곱해보면, 자기자신과 내적을 한 경우 결과가 1, 다른 직교벡터랑 내적을 한 경우 0 이 나오기 때문
	// 내적은 각 성분끼리의 곱을 합친 결과, 
	// 내적 결과값의 의미는 (벡터의 길이) x (길이) x (두 벡터가 이루는 각도의 cos 값) 

	//		 ( vR.x   vU.x   vF.x    0 ) 
	// R ==  ( vR.y   vU.y   vF.y    0 )
	//		 ( vR.z   vU.z   vF.z    0 )
	//       (   0     0       0     1 )

	Vec3 vR = Transform()->GetDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetDir(DIR::UP);
	Vec3 vF = Transform()->GetDir(DIR::FRONT);

	Matrix matRot = XMMatrixIdentity();
	matRot._11 = vR.x;  matRot._12 = vU.x;  matRot._13 = vF.x;
	matRot._21 = vR.y;	matRot._22 = vU.y;	matRot._23 = vF.y;
	matRot._31 = vR.z;	matRot._32 = vU.z;	matRot._33 = vF.z;

	// 카메라가 원점인 공간으로 이동, 카메라가 바라보는 방향을 z 축으로 회전하는 회전을 적용
	m_matView = matTrans * matRot;
	


	// 투영행렬
	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// 직교투영(Orthographic) 행렬 계산	
		m_matProj = XMMatrixOrthographicLH(m_Width, m_Width / m_AspectRatio, 1.f, m_Far);
	}
	else
	{
		// 원근투영(Perspective)
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}	
}

void CCamera::Render()
{
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;

	// 카메라가 보는 장면을 화면에 렌더링
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (false == (m_LayerCheck & (1 << i)))
			continue;

		Layer* pLayer = pCurLevel->GetLayer(i);
		pLayer->Render();
	}
}

void CCamera::LayerCheck(int _Idx)
{
	m_LayerCheck ^= (1 << _Idx);
}