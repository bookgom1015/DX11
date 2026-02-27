#include "pch.h"
#include "GammaCorrection.h"

#include "Device.h"
#include "AGraphicShader.h"

GammaCorrection::GammaCorrection() {}

GammaCorrection::~GammaCorrection() {}

int GammaCorrection::Init() {
	if (FAILED(Compile()))
		return E_FAIL;

	return S_OK;
}

void GammaCorrection::Apply() {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CopyResource(device->GetRenderTargetCopy(), device->GetScene());

	//context->RSSetState(mRasterizerState.Get());
	//context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	//context->OMSetBlendState(mBlendState.Get(), nullptr, 0xFFFFFFFF);

	//context->OMSetRenderTargets(1, &pBackBufferRtv, nullptr);

	auto srv = device->GetRenderTargetCopySrv();
	context->PSSetShaderResources(0, 1, &srv);
	//context->PSSetSamplers(
	//	0, Util::SamplerUtil::SamplerCount(), Util::SamplerUtil::GetSamplers());

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(mGammaCorrectionVS.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(mGammaCorrectionPS.Get(), nullptr, 0);

	context->Draw(6, 0);

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	context->PSSetShaderResources(0, _countof(nullSrvs), nullSrvs);
}

int GammaCorrection::Compile() {
	if (FAILED(AGraphicShader::CreateVertexShader(mGammaCorrectionVS, L"Shader\\gamma.fx", "VS_Gamma")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(mGammaCorrectionPS, L"Shader\\gamma.fx", "PS_Gamma")))
		return E_FAIL;

	return S_OK;
}