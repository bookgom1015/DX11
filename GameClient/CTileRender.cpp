#include "pch.h"
#include "CTileRender.h"

#include "AssetMgr.h"
#include "RenderMgr.h"
#include "CTransform.h"

#include "Device.h"
#include "Shadow.h"

CTileRender::CTileRender() : CRenderComponent(COMPONENT_TYPE::TILE_RENDER) {
	m_Buffer = new StructuredBuffer;
}

CTileRender::~CTileRender() {}

void CTileRender::FinalTick() {}

void CTileRender::Render() {
	m_Buffer->Binding(20);

	decltype(auto) mtrl = GetMaterial();
	mtrl->SetTexture(TEX_0, m_TileMap->GetAtlas());
	mtrl->SetScalar(INT_0, m_TileMap->GetRow());
	mtrl->SetScalar(INT_1, m_TileMap->GetCol());
	mtrl->SetScalar(INT_2, RenderMgr::GetInst()->GetLightCount());
	mtrl->Binding();

	auto srv = Shadow::sShadow->GetSrv();
	CONTEXT->PSSetShaderResources(6, 1, &srv);

	GetMesh()->Render();

	m_Buffer->Clear();

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	CONTEXT->PSSetShaderResources(6, _countof(nullSrvs), nullSrvs);
}



void CTileRender::SetTileMap(Ptr<ATileMap> _TileMap) {
	m_TileMap = _TileMap;
	if (m_TileMap == nullptr) return;

	// 이전 정보 리셋
	m_vecSpriteInfo.clear();	

	// 크키 조정
	UINT Row = m_TileMap->GetRow();
	UINT Col = m_TileMap->GetCol();
	Vec2 TileSize = m_TileMap->GetTileSize();
	Vec3 vScale = Vec3(TileSize.x * (float)Row, TileSize.y * (float)Col, 1.f);
	Transform()->SetRelativeScale(vScale);

	// TileMap 의 Sprite 의 UV 정보를 받아옴
	vector<Ptr<ASprite>> vecSprites = m_TileMap->GetSprites();

	for (int i = 0; i < vecSprites.size(); ++i) {		
		SpriteInfo info{};

		if (nullptr != vecSprites[i]) {
			info.LeftTop = vecSprites[i]->GetLeftTopUV();
			info.Slice = vecSprites[i]->GetSliceUV();
		}

		m_vecSpriteInfo.push_back(info);
	}

	// 구조화버퍼의 크기가 모자라거나 한번도 할당할 적이 없으면 메모리 확장 or 재확장
	if (m_Buffer->GetBufferSize() < sizeof(SpriteInfo) * m_vecSpriteInfo.size()) {
		m_Buffer->Create(
			static_cast<UINT>(sizeof(SpriteInfo)),
			static_cast<UINT>(m_vecSpriteInfo.size()),
			SB_TYPE::SRV_ONLY, 
			true);
	}

	// Sprite 들의 데이터를 구조화버퍼로 보내기
	m_Buffer->SetData(
		m_vecSpriteInfo.data(),
		static_cast<UINT>(sizeof(SpriteInfo) * m_vecSpriteInfo.size()));
}

void CTileRender::CreateMaterial() {
	static const wstring MeshName = L"RectMesh";
	static const wstring MtrlName = L"TileMtrl";
	static const wstring ShaderName = L"TileShader";
	static const wstring FilePath = L"Shader\\tile.fx";
	static const string VS = "VS_Tile";
	static const string PS = "PS_Tile";

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
			pShader->SetBSType(BS_TYPE::DEFAULT);
			pShader->SetRSType(RS_TYPE::CULL_NONE);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		// 찾은 or 생성한 쉐이더를 재질에 설정해주고, 재질도 에셋매니저에 등록한다.
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(pMtrl);
}