#include "pch.h"
#include "RenderMgr.h"

#include "Device.h"
#include "AssetMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

#include "GammaCorrection.h"
#include "ToneMapping.h"
#include "Shadow.h"
#include "Bloom.h"
#include "Blur.h"

#include "EditorMgr.h"
#include "LevelMgr.h"

bool RenderMgr::DebugRender = false;
bool RenderMgr::GammaEnabled = true;
bool RenderMgr::ToneEnabled = true;
bool RenderMgr::BloomEnabled = true;

EToneMapper::Type RenderMgr::ToneType = EToneMapper::E_ACES;


RenderMgr::RenderMgr() {
	mGamma = std::make_unique<GammaCorrection>();
	mTone = std::make_unique<ToneMapping>();
	mShadow = std::make_unique<Shadow>();
	mBloom = std::make_unique<Bloom>();
	mBlur = std::make_unique<Blur>();
}

RenderMgr::~RenderMgr() {}

void RenderMgr::Init() {
	m_DbgObj = new GameObject;
	m_DbgObj->AddComponent(new CTransform);
	m_DbgObj->AddComponent(new CMeshRender);
	m_DbgObj->MeshRender()->SetMaterial(FIND(AMaterial, L"DbgMtrl"));

	m_Light2DBuffer = new StructuredBuffer;

	mGamma->Init();
	mTone->Init();
	mShadow->Init();
	mBloom->Init();
	mBlur->Init();

	mCB = new ConstBuffer();
	mCB->Create(CB_TYPE::MATERIAL, sizeof(MtrlConst));	
}

void RenderMgr::Progress() {
	if (KEY_TAP(KEY::F9)) DebugRender ? DebugRender = false : DebugRender = true;
	
	// 레더링 시작전에 할 일
	Render_Start();

	if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing) {
		// 카메라 기반 렌더링
		if (nullptr == m_MainCam) return;

		ApplyShadow();

		// 타겟 설정
		Device::GetInst()->OMSetTarget();

		// 렌더타겟 클리어
		Device::GetInst()->ClearTarget();

		// 카메라를 이용해서 레벨안에 있는 물체들을 렌더링
		m_MainCam->SortObject();
		m_MainCam->Render();
	}
	else {
		// 카메라 기반 렌더링
		if (nullptr == m_EditorCam) return;

		ApplyShadow();

		// 타겟 설정
		Device::GetInst()->OMSetTarget();

		// 렌더타겟 클리어
		Device::GetInst()->ClearTarget();

		// 카메라를 이용해서 레벨안에 있는 물체들을 렌더링
		m_EditorCam->SortObject();
		m_EditorCam->Render();
	}

	Device::GetInst()->ResetTarget();

	Render_Post();

	// 디버그 렌더링 요청 처리
	if (DebugRender) Render_Debug();

	Render_End();
}

void RenderMgr::Render_Start() {
	// 등록받은 Light2D 의 광원 정보를 구조화버퍼에 담는다.
	// 구조화버퍼를 특정 t 레지스터에 바인딩 한다.
	vector<Light2DInfo> vecInfo{};
	UpdateLightInfos(vecInfo);

	// 등록된 광원이 최소 1개 이상인 경우에만 데이터를 구조화버퍼로 보낸다.
	if (!vecInfo.empty()) {
		// 구조화버퍼 공간이 모자라면 재확장 및 데이터 전달
		if (vecInfo.size() > m_Light2DBuffer->GetElementCount())
			m_Light2DBuffer->Create(
				static_cast<UINT>(sizeof(Light2DInfo)), 
				static_cast<UINT>(vecInfo.size()), 
				SB_TYPE::SRV_ONLY, 
				true, 
				vecInfo.data());

		// 공간이 여유가 있으면 바로 광원데이터 전달
		else
			m_Light2DBuffer->SetData(vecInfo.data(), static_cast<UINT>(sizeof(Light2DInfo) * vecInfo.size()));

		// t12 레지스터로 바인딩
		m_Light2DBuffer->Binding(12);
	}

	g_Global.Light2DCount = static_cast<int>(m_vecLight2D.size());

	// Globa 데이터를 상수버퍼를 통해서 b2 레지스터에 바인딩
	Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->SetData(&g_Global);
	Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->Binding();
}

void RenderMgr::Render_End() {
	// 구조화버퍼 클리어
	// 등록받았던 광원들 해제
	m_Light2DBuffer->Clear();
	m_vecLight2D.clear();
}

void RenderMgr::Render_Debug() {
	auto iter = m_DbgInfoList.begin();

	for (; iter != m_DbgInfoList.end();) {
		// Mesh 설정
		switch ((*iter).Shape)
		{
		case DBG_SHAPE::RECT:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"RectMesh_LineStrip"));
			break;
		case DBG_SHAPE::CIRCLE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"CircleMesh_LineStrip"));
			break;
		case DBG_SHAPE::CUBE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"CubeMesh"));
			break;
		case DBG_SHAPE::SPHERE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"SphereMesh"));
			break;
		}

		// Transform 설정

		if ((*iter).matWorld == XMMatrixIdentity())
		{
			m_DbgObj->Transform()->SetRelativePos((*iter).Pos);
			m_DbgObj->Transform()->SetRelativeScale((*iter).Scale);
			m_DbgObj->Transform()->SetRelativeRot((*iter).Rotation);
			m_DbgObj->Transform()->FinalTick();
		}
		else
		{
			m_DbgObj->Transform()->SetWorldMat((*iter).matWorld);
		}		
		
		// Material 설정
		m_DbgObj->MeshRender()->GetMaterial()->SetScalar(VEC4_0, (*iter).Color);

		if ((*iter).DepthTest)
			m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::LESS);
		else
			m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		// Render 요청
		m_DbgObj->Render();

		// 렌더링 시간 누적
		(*iter).Age += E_DT;

		// 최대 수명에 도달하면 정보 삭제
		if ((*iter).Life < (*iter).Age)
			iter = m_DbgInfoList.erase(iter);
		else
			++iter;
	}
}

void RenderMgr::Render_Post() {
	ApplyBloom();
	ApplyToneMapping();
	ApplyGammaCorrection();
}

void RenderMgr::UpdateLightInfos(std::vector<Light2DInfo>& infos) {
	for (int i = 0, idx = 0, end = m_vecLight2D.size(); i < end; ++i) {
		auto& light = m_vecLight2D[i];
		auto type = light->GetLightType();

		if (type == LIGHT_TYPE::SPOT 
			|| type == LIGHT_TYPE::LINE 
			|| type == LIGHT_TYPE::POINT) {
			auto pos = light->Transform()->GetRelativePos();
			auto dir = light->Light2D()->GetLightDir();

			auto rad = atan2(dir.y, dir.x);
			auto mat = XMMatrixRotationAxis(Vec4(0.f, 0.f, -1.f, 0.f), rad);
			auto axis = XMVector3Transform(Vec4(0.f, 1.f, 0.f, 0.f), mat);

			auto radius = light->Light2D()->GetRadius();

			if (type == LIGHT_TYPE::SPOT) {
				{
					auto focus = pos + dir;

					auto view = XMMatrixLookAtLH(
						Vec4(pos.x, pos.y, 0.f, 1.f),
						Vec4(focus.x, focus.y, 0.f, 1.f),
						axis);
					auto proj = XMMatrixPerspectiveFovLH(
						light->Light2D()->GetAngle(), 1.f, 1.f, radius);
					light->SetView(view);
					light->SetProj(proj);
				}

				light->SetBaseIndex(idx);
				light->SetSize(1);

				idx += 1;
			}
			else if (type == LIGHT_TYPE::LINE) {
				{
					auto halfL = light->GetLength() * 0.5f;
					Vec3 right = XMVector3Cross(dir, Vec3(0.f, 0.f, -1.f));

					auto posR = pos + right * halfL;
					auto posL = pos - right * halfL;

					auto focusR = posR + dir;
					auto focusL = posL + dir;

					auto view1 = XMMatrixLookAtLH(
						Vec4(posR.x, posR.y, 0.f, 1.f),
						Vec4(focusR.x, focusR.y, 0.f, 1.f),
						axis);
					auto proj1 = XMMatrixPerspectiveFovLH(
						XM_PI * 0.98f, 1.f, 1.f, radius);
					light->SetView(view1);
					light->SetProj(proj1);

					auto view2 = XMMatrixLookAtLH(
						Vec4(posL.x, posL.y, 0.f, 1.f),
						Vec4(focusL.x, focusL.y, 0.f, 1.f),
						axis);
					auto proj2 = XMMatrixPerspectiveFovLH(
						XM_PI * 0.98f, 1.f, 1.f, radius);
					light->SetView(view2, 1);
					light->SetProj(proj2, 1);
				}

				light->SetBaseIndex(idx);
				light->SetSize(2);

				idx += 2;
			}
			else {
				// +X
				{
					auto focus = pos + Vec3(1.f, 0.f, 0.f);

					auto view = XMMatrixLookAtLH(
						Vec4(pos.x, pos.y, 0.f, 1.f),
						Vec4(focus.x, focus.y, 0.f, 1.f),
						Vec4(0.f, 1.f, 0.f, 0.f));
					auto proj = XMMatrixPerspectiveFovLH(
						XM_PIDIV2, 1.f, 1.f, radius);
					light->SetView(view);
					light->SetProj(proj);
				}
				// -X
				{
					auto focus = pos + Vec3(-1.f, 0.f, 0.f);

					auto view = XMMatrixLookAtLH(
						Vec4(pos.x, pos.y, 0.f, 1.f),
						Vec4(focus.x, focus.y, 0.f, 1.f),
						Vec4(0.f, -1.f, 0.f, 0.f));
					auto proj = XMMatrixPerspectiveFovLH(
						XM_PIDIV2, 1.f, 1.f, radius);
					light->SetView(view, 1);
					light->SetProj(proj, 1);
				}
				// +Y
				{
					auto focus = pos + Vec3(0.f, 1.f, 0.f);

					auto view = XMMatrixLookAtLH(
						Vec4(pos.x, pos.y, 0.f, 1.f),
						Vec4(focus.x, focus.y, 0.f, 1.f),
						Vec4(-1.f, 0.f, 0.f, 0.f));
					auto proj = XMMatrixPerspectiveFovLH(
						XM_PIDIV2, 1.f, 1.f, radius);
					light->SetView(view, 2);
					light->SetProj(proj, 2);
				}
				// -Y
				{
					auto focus = pos + Vec3(0.f, -1.f, 0.f);

					auto view = XMMatrixLookAtLH(
						Vec4(pos.x, pos.y, 0.f, 1.f),
						Vec4(focus.x, focus.y, 0.f, 1.f),
						Vec4(1.f, 0.f, 0.f, 0.f));
					auto proj = XMMatrixPerspectiveFovLH(
						XM_PIDIV2, 1.f, 1.f, radius);
					light->SetView(view, 3);
					light->SetProj(proj, 3);
				}

				light->SetBaseIndex(idx);
				light->SetSize(4);

				idx += 4;
			}
		}
		else {
			light->SetBaseIndex(-1);
			light->SetSize(0);
		}

		infos.push_back(m_vecLight2D[i]->GetInfo());
	}
}

void RenderMgr::ApplyBloom() {
	if (!BloomEnabled) return;

	mBloom->Apply(
		mBlur.get(),
		Device::GetInst()->GetHDR(),
		Device::GetInst()->GetHDR_Rtv(),
		Device::GetInst()->GetHDR_Copy(),
		Device::GetInst()->GetHDR_Copy_Srv(),
		Device::GetInst()->GetHDR_Srv());
}

void RenderMgr::ApplyToneMapping() {
	MtrlConst constant;
	constant.iArr[0] = !ToneEnabled;
	constant.iArr[1] = ToneType;

	mCB->SetData(&constant);
	mCB->Binding();

	mTone->Apply();
}

void RenderMgr::ApplyGammaCorrection() {
	if (!GammaEnabled) return;
	
	mGamma->Apply();
}

void RenderMgr::ApplyShadow() {
	mShadow->Apply(m_vecLight2D);
}