#include "pch.h"
#include "CMeshRender.h"

#include "RenderMgr.h"

#include "Shadow.h"
#include "Device.h"

CMeshRender::CMeshRender() : CRenderComponent(EComponent::E_MeshRender) {}

CMeshRender::~CMeshRender() {}

void CMeshRender::FinalTick() {}

void CMeshRender::Render() {
	// Mesh or Shader 가 미설정 상태
	decltype(auto) mesh = GetMesh();
	decltype(auto) mtrl = GetMaterial();
	if (mesh == nullptr || mtrl == nullptr) return;

	mtrl->SetScalar(VEC4_1, m_Albedo);
	mtrl->SetScalar(INT_0, RenderMgr::GetInst()->GetLightCount());

	mtrl->Binding();

	auto srv = Shadow::sShadow->GetSrv();
	CONTEXT->PSSetShaderResources(6, 1, &srv);

	mesh->Render();
	
	mtrl->Clear();

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	CONTEXT->PSSetShaderResources(6, _countof(nullSrvs), nullSrvs);
}

void CMeshRender::SaveToLevelFile(FILE* const _FileStream) {
	CRenderComponent::SaveToLevelFile(_FileStream);

	fwrite(&m_Albedo, sizeof(m_Albedo), 1, _FileStream);
}

void CMeshRender::LoadFromLevelFile(FILE* const _FileStream) {
	CRenderComponent::LoadFromLevelFile(_FileStream);

	fread(&m_Albedo, sizeof(m_Albedo), 1, _FileStream);
}