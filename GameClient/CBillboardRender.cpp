#include "pch.h"
#include "CBillboardRender.h"

#include "AssetMgr.h"

CBillboardRender::CBillboardRender()
	: CRenderComponent(EComponent::E_BillboardRender) {}

CBillboardRender::~CBillboardRender() {}

void CBillboardRender::SaveToLevelFile(FILE* const _FileStream) {
	CRenderComponent::SaveToLevelFile(_FileStream);

	fwrite(&m_BillboardScale, sizeof(Vec2), 1, _FileStream);
	fwrite(&m_Albedo, sizeof(m_Albedo), 1, _FileStream);
	SaveAssetRef(_FileStream, m_Tex.Get());
}

void CBillboardRender::LoadFromLevelFile(FILE* const _FileStream) {
	CRenderComponent::LoadFromLevelFile(_FileStream);

	fread(&m_BillboardScale, sizeof(Vec2), 1, _FileStream);
	fread(&m_Albedo, sizeof(m_Albedo), 1, _FileStream);
	m_Tex = LoadAssetRef<ATexture>(_FileStream);
}

void CBillboardRender::FinalTick() {}

void CBillboardRender::Render() {
	decltype(auto) mesh = GetMesh();
	decltype(auto) mtrl = GetMaterial();
	if (GetMesh() == nullptr || mtrl == nullptr) return;

	auto scale = Transform()->GetRelativeScale();
	scale *= Vec3(m_BillboardScale.x, m_BillboardScale.y, 1.f);

	auto rot = Transform()->GetRelativeRot();

	mtrl->SetScalar(FLOAT_0, rot.z);
	mtrl->SetScalar(VEC2_0, Vec2(scale.x, scale.y));
	mtrl->SetScalar(VEC4_0, Vec4(m_Albedo.x, m_Albedo.y, m_Albedo.z, 1.f));
	mtrl->Binding();

	mesh->Render();

	mtrl->Clear();
}

void CBillboardRender::CreateMaterial() {
	// RectMesh 설정
	SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));

	// 재질 생성
	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(L"BillboardMtrl");

	// 찾는 재질이 없으면 생성한다.
	if (pMtrl == nullptr) {
		pMtrl = NEW AMaterial;
		pMtrl->SetName(L"BillboardMtrl");	

		// 쉐이더를 찾아서 재질에 세팅해준다.
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(L"BillboardShader");

		// 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해둔다
		if (pShader == nullptr) {
			pShader = NEW AGraphicShader;
			pShader->SetName(L"BillboardShader");
			pShader->CreateVertexShader(L"Shader\\billboard.fx", "VS_Billboard");
			pShader->CreatePixelShader(L"Shader\\billboard.fx", "PS_Billboard");
			pShader->SetBSType(EBlendState::E_Default);
			pShader->SetRSType(ERasterizerState::E_CullNone);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		// 찾은 or 생성한 쉐이더를 재질에 설정해주고, 재질도 에셋매니저에 등록한다.
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(ERenderDomain::E_Opaque);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(pMtrl);	
}
