#include "pch.h"
#include "Shadow.h"

#include "LevelMgr.h"
#include "RenderMgr.h"
#include "Device.h"

Shadow* Shadow::sShadow = nullptr;

Shadow::Shadow() {
	sShadow = this;
}

Shadow::~Shadow() {}

int Shadow::Init() {
	if (FAILED(Compile())) return E_FAIL;
	if (FAILED(BuildResources())) return E_FAIL;
	if (FAILED(BuildPipelines())) return E_FAIL;

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = 2048;
	mViewport.Height = 2048;
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;

	return S_OK;
}

void Shadow::Apply(const vector<Ptr<CLight2D>>& lights) {
	CONTEXT->RSSetViewports(1, &mViewport);

	CONTEXT->RSSetState(mRasterizerState.Get());
	CONTEXT->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	CONTEXT->OMSetBlendState(mBlendState.Get(), nullptr, 0xFFFFFFFF);

	CONTEXT->IASetInputLayout(mLayout.Get());
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CONTEXT->VSSetShader(mShadowVS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(mShadowGS.Get(), nullptr, 0);
	CONTEXT->PSSetShader(mShadowPS.Get(), nullptr, 0);

	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	auto grounds = pCurLevel->GetLayer(ELevelLayer::E_Ground);
	auto objects = grounds->GetAllObjects();

	for (size_t i = 0, end = lights.size(); i < end; ++i) {
		auto& light = lights[i];

		if (light->GetLightType() == ELight::E_Spot)
			CalcSpotLight(light, objects);
		else if (light->GetLightType() == ELight::E_Line)
			CalcLineLight(light, objects);
		else if (light->GetLightType() == ELight::E_Point)
			CalcPointLight(light, objects);
	}

	CONTEXT->VSSetShader(nullptr, nullptr, 0);
	CONTEXT->GSSetShader(nullptr, nullptr, 0);
	CONTEXT->PSSetShader(nullptr, nullptr, 0);
}

int Shadow::Compile() {
	if (FAILED(AGraphicShader::CreateVertexShader(
		mShadowVS, mLayout, L"Shader\\shadow.fx", "VS_Shadow")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreateGometryShader(
		mShadowGS, L"Shader\\shadow.fx", "GS_Shadow")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(
		mShadowPS, L"Shader\\shadow.fx", "PS_Shadow")))
		return E_FAIL;

	return S_OK;
}

int Shadow::BuildResources() {
	D3D11_TEXTURE2D_DESC Desc{};
	Desc.ArraySize = ArraySize;
	Desc.Width = 2048;
	Desc.Height = 2048;
	Desc.Format = DXGI_FORMAT_R32_TYPELESS;
	Desc.CPUAccessFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	Desc.MipLevels = 1;
	Desc.MiscFlags = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	if (FAILED(DEVICE->CreateTexture2D(&Desc, nullptr, mDepthMapArray.GetAddressOf())))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC ds{};
	ds.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	ds.Format = DXGI_FORMAT_D32_FLOAT;
	ds.Texture2DArray.MipSlice = 0;
	ds.Texture2DArray.ArraySize = 1;

	for (UINT i = 0; i < ArraySize; ++i) {
		ds.Texture2DArray.FirstArraySlice = i;

		if (FAILED(DEVICE->CreateDepthStencilView(
			mDepthMapArray.Get(), &ds, mDsvs[i].GetAddressOf())))
			return E_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC sr{};
	sr.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	sr.Format = DXGI_FORMAT_R32_FLOAT;
	sr.Texture2DArray.MipLevels = 1;
	sr.Texture2DArray.MostDetailedMip = 0;
	sr.Texture2DArray.ArraySize = ArraySize;
	sr.Texture2DArray.FirstArraySlice = 0;

	if (FAILED(DEVICE->CreateShaderResourceView(
		mDepthMapArray.Get(), &sr, mSrv.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

int Shadow::BuildPipelines() {
	auto device = Device::GetInst();

	D3D11_RASTERIZER_DESC rs{};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_NONE;
	rs.FrontCounterClockwise = FALSE;
	rs.DepthClipEnable = TRUE;
	DEVICE->CreateRasterizerState(&rs, &mRasterizerState);

	D3D11_DEPTH_STENCIL_DESC ds{};
	ds.DepthEnable = TRUE;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.StencilEnable = FALSE;
	DEVICE->CreateDepthStencilState(&ds, &mDepthStencilState);

	D3D11_BLEND_DESC bd{};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = FALSE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DEVICE->CreateBlendState(&bd, &mBlendState);

	return S_OK;
}

void Shadow::CalcSpotLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs) {
	auto idx = light->GetBaseIndex();

	CONTEXT->ClearDepthStencilView(
		mDsvs[idx].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	CONTEXT->OMSetRenderTargets(0, nullptr, mDsvs[idx].Get());

	g_Trans.matView = light->GetView();
	g_Trans.matProj = light->GetProj();

	for (const auto& obj : objs) {
		g_Trans.matWorld = obj->Transform()->GetWorldMat();

		// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
		Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
		Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();

		obj->GetRenderCom()->GetMesh()->Render();
	}
}

void Shadow::CalcLineLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs) {
	auto idx = light->GetBaseIndex();

	for (int i = 0, end = light->GetSize(); i < end; ++i, ++idx) {
		CONTEXT->ClearDepthStencilView(
			mDsvs[idx].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		CONTEXT->OMSetRenderTargets(0, nullptr, mDsvs[idx].Get());

		g_Trans.matView = light->GetView(i);
		g_Trans.matProj = light->GetProj(i);

		for (const auto& obj : objs) {
			g_Trans.matWorld = obj->Transform()->GetWorldMat();

			// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
			Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
			Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();

			obj->GetRenderCom()->GetMesh()->Render();
		}
	}
}

void Shadow::CalcPointLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs) {
	auto idx = light->GetBaseIndex();

	for (int i = 0, end = light->GetSize(); i < end; ++i, ++idx) {
		CONTEXT->ClearDepthStencilView(
			mDsvs[idx].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		CONTEXT->OMSetRenderTargets(0, nullptr, mDsvs[idx].Get());

		g_Trans.matView = light->GetView(i);
		g_Trans.matProj = light->GetProj(i);

		for (const auto& obj : objs) {
			g_Trans.matWorld = obj->Transform()->GetWorldMat();

			// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
			Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
			Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();

			obj->GetRenderCom()->GetMesh()->Render();
		}
	}
}