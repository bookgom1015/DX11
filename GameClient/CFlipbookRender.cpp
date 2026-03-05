#include "pch.h"
#include "CFlipbookRender.h"

#include "AssetMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"

CFlipbookRender::CFlipbookRender()
	: CRenderComponent(COMPONENT_TYPE::FLIPBOOK_RENDER),
	m_FPS(0.f),
	m_AccTime(0.f),
	m_CurSprite(0),
	m_CurFlipbook(0),
	m_Finish(true),
	m_RepeatCount(0) {
}

CFlipbookRender::~CFlipbookRender() {}

void CFlipbookRender::FinalTick() {
	if (CheckFinish()) return;

	float fLmit = 1.f / m_FPS;
	m_AccTime += DT;

	if (fLmit < m_AccTime) {
		m_AccTime -= fLmit;
		++m_CurSprite;

		auto count = static_cast<int>(m_vecFlipbook[m_CurFlipbook]->GetSpriteCount());
		if (count <= m_CurSprite) {
			m_Finish = true;
			--m_CurSprite;
		}
	}
}

void CFlipbookRender::Render() {
	Ptr<AFlipbook> pCurFlipbook = m_vecFlipbook[m_CurFlipbook];
	Ptr<ASprite> pCurSprite = pCurFlipbook->GetSprite(m_CurSprite);

	decltype(auto) mtrl = GetMaterial();

	mtrl->SetTexture(TEX_0, pCurSprite->GetAtlas());
	mtrl->SetScalar(VEC2_0, pCurSprite->GetLeftTopUV());
	mtrl->SetScalar(VEC2_1, pCurSprite->GetSliceUV());
	mtrl->SetScalar(INT_0, RenderMgr::GetInst()->GetLightCount());

	mtrl->Binding();
	GetMesh()->Render();
}

void CFlipbookRender::CreateMaterial() {
	static const wstring MeshName = L"RectMesh";
	static const wstring MtrlName = L"FlipbookMtrl";
	static const wstring ShaderName = L"FlipbookShader";
	static const wstring FilePath = L"Shader\\flipbook.fx";
	static const string VS = "VS_Flipbook";
	static const string PS = "PS_Flipbook";

	// RectMesh 설정
	SetMesh(AssetMgr::GetInst()->Find<AMesh>(MeshName));

	// 재질 생성
	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	// 찾는 재질이 없으면 생성한다.
	if (pMtrl == nullptr) {
		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);

		// 쉐이더를 찾아서 재질에 세팅해준다.
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		// 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해둔다
		if (pShader == nullptr) {
			pShader = new AGraphicShader;
			pShader->SetName(ShaderName);
			pShader->CreateVertexShader(FilePath, VS);
			pShader->CreatePixelShader(FilePath, PS);
			pShader->SetBSType(EBlendState::E_Default);
			pShader->SetRSType(ERasterizerState::E_CullNone);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		// 찾은 or 생성한 쉐이더를 재질에 설정해주고, 재질도 에셋매니저에 등록한다.
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(pMtrl);
}

Ptr<ASprite> CFlipbookRender::GetCurrentSprite() {
	Ptr<AFlipbook> pCurFlipbook = m_vecFlipbook[m_CurFlipbook];
	return pCurFlipbook->GetSprite(m_CurSprite);
}

bool CFlipbookRender::CheckFinish() {
	if (m_Finish) {
		if (m_RepeatCount > 0) {
			m_CurSprite = 0;
			m_Finish = false;
			--m_RepeatCount;
			return false;
		}
		else if (m_RepeatCount == -1) {
			m_CurSprite = 0;
			m_Finish = false;
			return false;
		}
		else {
			return true;
		}
	}

	return false;
}