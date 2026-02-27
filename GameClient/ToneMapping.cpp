#include "pch.h"
#include "ToneMapping.h"

#include "Device.h"
#include "AGraphicShader.h"

ToneMapping::ToneMapping() {}

ToneMapping::~ToneMapping() {}

int ToneMapping::Init() {
	if (FAILED(Compile()))
		return E_FAIL;

	if (FAILED(BuildPipelines()))
		return E_FAIL;

	return S_OK;
}

void ToneMapping::Apply() {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->RSSetState(mRasterizerState.Get());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->OMSetBlendState(mBlendState.Get(), nullptr, 0xFFFFFFFF);

	auto rtv = device->GetScene_Rtv();
	context->OMSetRenderTargets(1, &rtv, nullptr);

	//ID3D11ShaderResourceView* srvs[] = { mhIntermediateMapSrv.Get() };
	//context->PSSetShaderResources(0, 1, srvs);

	auto srv = device->GetHDR_Srv();
	context->PSSetShaderResources(0, 1, &srv);
	//context->PSSetSamplers(
	//	0, Util::SamplerUtil::SamplerCount(), Util::SamplerUtil::GetSamplers());

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(mToneMappingVS.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(mToneMappingPS.Get(), nullptr, 0);

	context->Draw(6, 0);

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	context->PSSetShaderResources(0, _countof(nullSrvs), nullSrvs);
}

int ToneMapping::Compile() {
	if (FAILED(AGraphicShader::CreateVertexShader(mToneMappingVS, L"Shader\\tone.fx", "VS_Tone")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(mToneMappingPS, L"Shader\\tone.fx", "PS_Tone")))
		return E_FAIL;

	return S_OK;
}

int ToneMapping::BuildPipelines() {
	auto device = Device::GetInst();

	D3D11_RASTERIZER_DESC rs{};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_BACK;
	rs.FrontCounterClockwise = FALSE;
	rs.DepthClipEnable = TRUE;
	DEVICE->CreateRasterizerState(&rs, &mRasterizerState);

	D3D11_DEPTH_STENCIL_DESC ds{};
	ds.DepthEnable = FALSE;
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