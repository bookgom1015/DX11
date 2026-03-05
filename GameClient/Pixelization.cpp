#include "pch.h"
#include "Pixelization.h"

#include "Device.h"
#include "AGraphicShader.h"

Pixelization::Pixelization() {}

Pixelization::~Pixelization() {}

int Pixelization::Init() {
	if (FAILED(Compile()))
		return E_FAIL;

	return S_OK;
}

void Pixelization::Apply() {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CopyResource(device->GetRenderTargetCopy(), device->GetScene());

	auto srv = device->GetRenderTargetCopySrv();
	context->PSSetShaderResources(0, 1, &srv);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(mPixelizationVS.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(mPixelizationPS.Get(), nullptr, 0);

	context->Draw(6, 0);

	ID3D11ShaderResourceView* nullSrvs[1] = {};
	context->PSSetShaderResources(0, _countof(nullSrvs), nullSrvs);
}

int Pixelization::Compile() {
	if (FAILED(AGraphicShader::CreateVertexShader(
		mPixelizationVS, L"Shader\\pixelization.fx", "VS_Pixel")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(
		mPixelizationPS, L"Shader\\pixelization.fx", "PS_Pixel")))
		return E_FAIL;

	return S_OK;
}