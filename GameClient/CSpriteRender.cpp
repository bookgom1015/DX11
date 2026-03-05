#include "pch.h"
#include "CSpriteRender.h"

#include "AssetMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"

#include "Shadow.h"
#include "Device.h"

CSpriteRender::CSpriteRender() : CRenderComponent(COMPONENT_TYPE::SPRITE_RENDER) {}

CSpriteRender::~CSpriteRender() {}

void CSpriteRender::FinalTick() {}

void CSpriteRender::Render() {	
	static float elapsedTime{};
	elapsedTime += DT;

	decltype(auto) mtrl = GetMaterial();
	mtrl->SetTexture(TEX_0, m_Sprite->GetAtlas());
	mtrl->SetScalar(VEC2_0, m_Sprite->GetLeftTopUV());
	mtrl->SetScalar(VEC2_1, m_Sprite->GetSliceUV());
	mtrl->SetScalar(VEC4_1, m_Albedo);
	mtrl->SetScalar(FLOAT_0, elapsedTime);
	mtrl->SetScalar(INT_0, (int)m_GlitchEffectEnabled);
	mtrl->SetScalar(INT_1, (int)m_RelicEffectEnabled);
	mtrl->SetScalar(INT_2, (int)m_Emit);

	mtrl->Binding();

	auto srv = Shadow::sShadow->GetSrv();
	CONTEXT->PSSetShaderResources(6, 1, &srv);

	GetMesh()->Render();

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	CONTEXT->PSSetShaderResources(6, _countof(nullSrvs), nullSrvs);
}

void CSpriteRender::CreateMaterial() {
	static const wstring MeshName = L"RectMesh";
	static const wstring MtrlName = L"SpriteMtrl";
	static const wstring ShaderName = L"SpriteShader";
	static const wstring FilePath = L"Shader\\sprite.fx";
	static const string VS = "VS_Sprite";	
	static const string PS = "PS_Sprite";

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
		if (nullptr == pShader) {
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