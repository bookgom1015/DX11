#include "pch.h"
#include "Vignette.h"

#include "Device.h"
#include "AGraphicShader.h"

Vignette::Vignette() {}

Vignette::~Vignette() {}

int Vignette::Init() {
	if (FAILED(Compile()))
		return E_FAIL;

	return S_OK;
}

void Vignette::Apply() {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CopyResource(device->GetRenderTargetCopy(), device->GetScene());

	auto srv = device->GetRenderTargetCopySrv();
	context->PSSetShaderResources(0, 1, &srv);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(mVignetteVS.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(mVignettePS.Get(), nullptr, 0);

	context->Draw(6, 0);

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	context->PSSetShaderResources(0, _countof(nullSrvs), nullSrvs);
}

int Vignette::Compile() {
	if (FAILED(AGraphicShader::CreateVertexShader(
		mVignetteVS, L"Shader\\vignette.fx", "VS_Vignette")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(
		mVignettePS, L"Shader\\vignette.fx", "PS_Vignette")))
		return E_FAIL;
}